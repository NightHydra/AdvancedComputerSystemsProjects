#ifndef BLOCKED_BLOOM_FILTER_H
#define BLOCKED_BLOOM_FILTER_H

#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <cstdint>
#include "xxhash.h"

// --- Constants from the provided header ---
#ifndef SET_SIZE
#define SET_SIZE (5000000)
#endif
#ifndef TARGET_FPR
// The optimal number of hash functions (k) depends on m/n and FPR
#define TARGET_FPR (0.01)
#endif
#ifndef LOAD_FACTOR
#define LOAD_FACTOR (0.5) // Not strictly used for insertion, but for sizing M
#endif
#ifndef FINGERPRINT_WIDTH
#define FINGERPRINT_WIDTH (8) // For BBF, this is typically the block size in bits
#endif
// ------------------------------------------

// --- Blocked Bloom Filter Specific Constants ---

// The size of a block in bits (m_b). Often set to 512 bits (64 bytes) to match
// a common CPU L1 cache line or a multiple thereof for optimal performance.
#define BLOCK_SIZE_BITS (512)
#define BLOCK_SIZE_BYTES (BLOCK_SIZE_BITS / 8)

// Optimal number of hash functions (k). For a given M (total bits) and N (keys):
// k = (M / N) * ln(2)
// Since we calculate M based on N and a desired k, we fix k here.
#define NUM_HASHES (4) // k (typically 3-7 for optimal performance)

// --- Type Definitions ---
using BlockT = uint8_t; // We store the filter as a flat array of bytes

// The mask to extract the bit offset within a block
const uint64_t BIT_OFFSET_MASK = BLOCK_SIZE_BITS - 1;

/**
 * @brief A macro for the hash function. We use XXH64 twice with different seeds
 * to generate the two independent hash values needed for the standard Bloom filter
 * double hashing trick (H1 and H2).
 * @param data A pointer to the data to hash
 * @param seed The seed for the hash function
 */
#define HASH_VALUE(data, seed) (XXH64(data, sizeof(KeyT), seed))

template <class KeyT>
class BlockedBloomFilter {
public:
    /**
     * @brief Constructs the Blocked Bloom filter from a set of keys.
     * @param keys A constant reference to the vector of keys to insert.
     */
    BlockedBloomFilter(const std::vector<KeyT>& keys);

    /**
     * @brief Destructor.
     */
    ~BlockedBloomFilter();

    /**
     * @brief Checks if a key exists in the filter.
     * @param key The key to check.
     * @return true if the key is likely present, false otherwise.
     */
    bool exist(KeyT key) const;

private:
    // --- Configuration Parameters ---
    size_t N; // Number of keys (size of the input set)
    size_t M; // Total number of bits (M = N / LOAD_FACTOR)
    size_t NumBlocks; // Total number of blocks (M / BLOCK_SIZE_BITS)

    // The backing array (the filter itself)
    std::vector<BlockT> table; // Size is NumBlocks * BLOCK_SIZE_BYTES

    // --- Helper Functions for Hashing and Indexing ---

    /**
     * @brief Calculates the index of the block and two hash values (H1, H2) for double hashing.
     *
     * @param key The key to hash.
     * @param block_idx Output for the index of the block to use.
     * @param h1 Output for the first hash value.
     * @param h2 Output for the second hash value.
     */
    void get_hashes(KeyT key, size_t& block_idx, uint64_t& h1, uint64_t& h2) const {
        uint64_t full_hash = HASH_VALUE(&key, 0);

        // 1. Determine the Block Index (i)
        // Use the upper bits of the hash for block selection
        block_idx = (full_hash >> (64 - INDEX_WIDTH)) % NumBlocks;

        // 2. Determine the two inner hash values (H1 and H2)
        // Use the lower 64 bits of the hash for the standard double hashing trick,
        // but only the bits needed to select a bit *within* the block.
        // H1 and H2 are used to calculate the k bit positions.
        h1 = full_hash;
        h2 = HASH_VALUE(&key, 1); // Use a different seed for the second hash
    }

    /**
     * @brief Calculates the j-th bit position within the block using the double hashing trick.
     *
     * @param h1 The first hash value.
     * @param h2 The second hash value.
     * @param j The hash iteration index (0 to NUM_HASHES - 1).
     * @return The bit offset within the block (0 to BLOCK_SIZE_BITS - 1).
     */
    size_t get_bit_offset(uint64_t h1, uint64_t h2, size_t j) const {
        // Double Hashing: (H1 + j * H2)
        // Mask with BLOCK_SIZE_BITS - 1 to constrain the position to the block size.
        uint64_t combined_hash = h1 + (uint64_t)j * h2;
        return (combined_hash & BIT_OFFSET_MASK);
    }

    /**
     * @brief Gets a reference to the byte containing a specific bit within the table.
     * @param block_idx The index of the block.
     * @param bit_offset The offset of the bit within the block (0 to BLOCK_SIZE_BITS - 1).
     * @return A reference to the containing byte.
     */
    BlockT& get_byte_ref(size_t block_idx, size_t bit_offset) {
        size_t byte_offset = bit_offset / 8;
        return table[block_idx * BLOCK_SIZE_BYTES + byte_offset];
    }

    /**
     * @brief Const version of get_byte_ref.
     */
    const BlockT& get_byte_ref(size_t block_idx, size_t bit_offset) const {
        size_t byte_offset = bit_offset / 8;
        return table[block_idx * BLOCK_SIZE_BYTES + byte_offset];
    }
};

// --- Implementation Details ---

template <class KeyT>
BlockedBloomFilter<KeyT>::BlockedBloomFilter(const std::vector<KeyT>& keys)
    : N(keys.size()) {

    if (N == 0) {
        M = 0;
        NumBlocks = 0;
        return;
    }

    // 1. Determine M (Total Bits) and NumBlocks
    // The optimal number of bits M required for a given N keys and FPR (p) is:
    // M = ceil((-N * ln(p)) / (ln(2)^2))

    double ln2 = std::log(2.0);
    M = std::ceil((-N * std::log(TARGET_FPR)) / (ln2 * ln2));

    // Ensure M is a multiple of BLOCK_SIZE_BITS
    NumBlocks = std::ceil(M / (double)BLOCK_SIZE_BITS);
    M = NumBlocks * BLOCK_SIZE_BITS;

    // For the hashing helper:
    // This is the number of bits required to address the blocks (log2(NumBlocks)).
    const size_t INDEX_WIDTH = std::ceil(std::log2(NumBlocks));

    // 2. Initialize the table
    table.resize(NumBlocks * BLOCK_SIZE_BYTES, 0);

    // 3. Insert all keys
    for (const auto& key : keys) {
        size_t block_idx;
        uint64_t h1, h2;
        get_hashes(key, block_idx, h1, h2);

        // Set the k bits in the single selected block
        for (size_t j = 0; j < NUM_HASHES; ++j) {
            size_t bit_offset = get_bit_offset(h1, h2, j);

            // Calculate the position of the bit within the byte
            size_t bit_in_byte = bit_offset % 8;

            // Set the bit: table[byte_index] |= (1 << bit_in_byte)
            get_byte_ref(block_idx, bit_offset) |= (1 << bit_in_byte);
        }
    }
}

template <class KeyT>
BlockedBloomFilter<KeyT>::~BlockedBloomFilter() {
    // std::vector handles memory
}

template <class KeyT>
bool BlockedBloomFilter<KeyT>::exist(KeyT key) const {
    if (NumBlocks == 0) return false;

    size_t block_idx;
    uint64_t h1, h2;
    get_hashes(key, block_idx, h1, h2);

    // Check all k bit positions within the single selected block
    for (size_t j = 0; j < NUM_HASHES; ++j) {
        size_t bit_offset = get_bit_offset(h1, h2, j);

        // Calculate the position of the bit within the byte
        size_t bit_in_byte = bit_offset % 8;

        // Check the bit: (table[byte_index] & (1 << bit_in_byte)) == 0
        if ((get_byte_ref(block_idx, bit_offset) & (1 << bit_in_byte)) == 0) {
            // Found a zero bit, key is definitely NOT present
            return false;
        }
    }

    // All k bits were set, key is likely present
    return true;
}

// Ensure the class is instantiated for common types if needed
// template class BlockedBloomFilter<uint64_t>;

#endif //BLOCKED_BLOOM_FILTER_H