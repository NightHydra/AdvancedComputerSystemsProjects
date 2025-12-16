//
// Created by Alek on 12/16/2025.
//

#ifndef CUCKOO_FILTER_H
#define CUCKOO_FILTER_H

#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include "xxhash.h" // Assume this is available

// --- Constants from the provided header ---
#ifndef SET_SIZE
#define SET_SIZE (5000000)
#endif
#ifndef TARGET_FPR
#define TARGET_FPR (1) // For Cuckoo, target FPR determines fingerprint width
#endif
#ifndef LOAD_FACTOR
#define LOAD_FACTOR (0.95) // Cuckoo filters typically support higher load factors (up to ~95%)
#endif
#ifndef FINGERPRINT_WIDTH
#define FINGERPRINT_WIDTH (12)
#endif
// ------------------------------------------

// --- Cuckoo Filter Specific Constants ---
#define SLOTS_PER_BUCKET (4) // Typical value for Cuckoo Filter: 2, 4, 8

// Determine the type for fingerprints and indices based on constants
using FingerprintT = uint16_t;

// The mask to extract the FP from the full hash value
const uint64_t FINGERPRINT_MASK = (1ULL << FINGERPRINT_WIDTH) - 1;

/**
 * @brief A macro for the hash function to make it more clear in the code.
 * @param data A pointer to the data to hash
 */
#define HASH_VALUE(data) (XXH64(data, sizeof(KeyT), 0))

template <class KeyT>
class CuckooFilter {
public:
    /**
     * @brief Constructs the Cuckoo filter. Keys are inserted in the constructor.
     * @param keys A constant reference to the vector of keys to insert.
     */
    CuckooFilter(const std::vector<KeyT>& keys);

    /**
     * @brief Destructor.
     */
    ~CuckooFilter();

    /**
     * @brief Checks if a key exists in the filter.
     * @param key The key to check.
     * @return true if the key is likely present, false otherwise.
     */
    bool exist(KeyT key) const;

    /**
     * @brief Attempts to insert a single key into the filter.
     * @param key The key to insert.
     * @return true on successful insertion, false if the filter is full/loop detected.
     */
    bool insert(KeyT key);

private:
    // --- Configuration Parameters ---
    size_t N; // Number of keys (size of the input set)
    size_t M; // Total number of buckets (M = ceil(N / (LOAD_FACTOR * SLOTS_PER_BUCKET)))
    size_t MAX_KICKS = 500; // Max reassignments allowed before declaring failure/resizing

    // The backing array for the fingerprints (the actual filter)
    // Structure: std::vector<std::array<FingerprintT, SLOTS_PER_BUCKET>>
    // For simplicity and direct indexing, we use a flat vector:
    std::vector<FingerprintT> table; // Size is M * SLOTS_PER_BUCKET

    // --- Helper Functions for Hashing and Indexing ---

    /**
     * @brief Calculates a hash value and the raw fingerprint for a key.
     * @param key The key to hash.
     * @param full_hash The output for the full 64-bit hash.
     * @return The raw (unassigned) fingerprint.
     */
    FingerprintT get_raw_fingerprint(KeyT key, uint64_t& full_hash) const {
        full_hash = HASH_VALUE(&key);
        // Use the lower FINGERPRINT_WIDTH bits of the hash as the raw fingerprint
        return static_cast<FingerprintT>(full_hash & FINGERPRINT_MASK);
    }

    /**
     * @brief Calculates the two potential bucket indices for a key.
     * The second index is calculated using a XOR/PRNG combination of the first index and the fingerprint.
     *
     * @param fp The raw fingerprint of the key.
     * @param i1 The first bucket index.
     * @return The second bucket index.
     */
    size_t get_second_index(FingerprintT fp, size_t i1) const {
        // The standard Cuckoo Filter calculation for the second choice:
        // i2 = i1 XOR Hash(Fingerprint)

        // This is a common and robust implementation
        // Note: The hash function for the fingerprint must be *different*
        // from the key's main hash to ensure independence.
        uint64_t fp_hash = XXH64(&fp, sizeof(FingerprintT), 1); // Seed '1' for a different hash

        return (i1 ^ (fp_hash % M)) % M;
    }

    /**
     * @brief Calculates the first potential bucket index (i1).
     * @param full_hash The full 64-bit hash of the key.
     * @return The first bucket index.
     */
    size_t get_first_index(uint64_t full_hash) const {
        return (uint32_t)(full_hash) % M;
    }

    /**
     * @brief Accesses a specific slot in the flat table.
     * @param bucket_idx The index of the bucket (0 to M-1).
     * @param slot_in_bucket The slot within the bucket (0 to SLOTS_PER_BUCKET-1).
     * @return A reference to the fingerprint slot.
     */
    FingerprintT& get_slot(size_t bucket_idx, size_t slot_in_bucket) {
        return table[bucket_idx * SLOTS_PER_BUCKET + slot_in_bucket];
    }

    /**
     * @brief Const access to a specific slot in the flat table.
     */
    const FingerprintT& get_slot(size_t bucket_idx, size_t slot_in_bucket) const {
        return table[bucket_idx * SLOTS_PER_BUCKET + slot_in_bucket];
    }
};

// --- Implementation Details ---

template <class KeyT>
CuckooFilter<KeyT>::CuckooFilter(const std::vector<KeyT>& keys)
    : N(keys.size()) {

    if (N == 0) {
        M = 0;
        return;
    }

    // M: Number of buckets. Calculate based on load factor and slots per bucket.
    // M = ceil(N / (LOAD_FACTOR * SLOTS_PER_BUCKET))
    M = std::ceil(N / (LOAD_FACTOR * SLOTS_PER_BUCKET));

    // Ensure M is at least 1
    if (M == 0) M = 1;

    // Initialize table: M buckets * SLOTS_PER_BUCKET total size
    table.resize(M * SLOTS_PER_BUCKET, 0);

    // Insert all keys
    for (const auto& key : keys) {
        if (!insert(key)) {
            // Construction failed (too many kicks/loop detected).
            // A full implementation would now resize the filter and re-insert.
            // For this reference, we throw.
            throw std::runtime_error("Cuckoo Filter construction failed: Max kicks exceeded. Resize required.");
        }
    }
}

template <class KeyT>
CuckooFilter<KeyT>::~CuckooFilter() {
    // std::vector handles memory
}

template <class KeyT>
bool CuckooFilter<KeyT>::insert(KeyT key) {
    uint64_t full_hash;
    FingerprintT fp = get_raw_fingerprint(key, full_hash);

    size_t i1 = get_first_index(full_hash);
    size_t i2 = get_second_index(fp, i1);

    // 1. Try to insert into i1
    for (size_t s = 0; s < SLOTS_PER_BUCKET; ++s) {
        if (get_slot(i1, s) == 0) {
            get_slot(i1, s) = fp;
            return true;
        }
    }

    // 2. Try to insert into i2
    for (size_t s = 0; s < SLOTS_PER_BUCKET; ++s) {
        if (get_slot(i2, s) == 0) {
            get_slot(i2, s) = fp;
            return true;
        }
    }

    // 3. Cuckoo: Start kicking
    size_t current_idx = (std::rand() % 2 == 0) ? i1 : i2; // Randomly choose starting bucket
    FingerprintT current_fp = fp;

    for (size_t num_kicks = 0; num_kicks < MAX_KICKS; ++num_kicks) {
        // Randomly choose a slot/fingerprint to kick out (i.e., overwrite)
        size_t slot_to_kick = std::rand() % SLOTS_PER_BUCKET;

        // Swap: The current fingerprint takes the place of the kicked one
        std::swap(current_fp, get_slot(current_idx, slot_to_kick));

        // Calculate the alternate index for the kicked fingerprint
        current_idx = get_second_index(current_fp, current_idx);

        // Try to insert the kicked fingerprint (current_fp) into its new alternative bucket (current_idx)
        for (size_t s = 0; s < SLOTS_PER_BUCKET; ++s) {
            if (get_slot(current_idx, s) == 0) {
                get_slot(current_idx, s) = current_fp;
                return true;
            }
        }
    }

    // Too many kicks/loop detected
    return false;
}

template <class KeyT>
bool CuckooFilter<KeyT>::exist(KeyT key) const {
    if (M == 0) return false;

    uint64_t full_hash;
    // Get the raw fingerprint and the full hash value
    FingerprintT fp = get_raw_fingerprint(key, full_hash);

    // Calculate the two potential bucket indices
    size_t i1 = get_first_index(full_hash);
    size_t i2 = get_second_index(fp, i1);

    // Check if the fingerprint exists in i1
    for (size_t s = 0; s < SLOTS_PER_BUCKET; ++s) {
        if (get_slot(i1, s) == fp) {
            return true;
        }
    }

    // Check if the fingerprint exists in i2
    for (size_t s = 0; s < SLOTS_PER_BUCKET; ++s) {
        if (get_slot(i2, s) == fp) {
            return true;
        }
    }

    return false;
}

// Ensure the class is instantiated for common types if needed,
// e.g., template class CuckooFilter<uint64_t>;

#endif //CUCKOO_FILTER_H
