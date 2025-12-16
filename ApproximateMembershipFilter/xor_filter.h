#ifndef XOR_FILTER_H
#define XOR_FILTER_H

#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>
#include "xxhash.h" // Assume this is available

// --- Constants from the provided header ---
#ifndef SET_SIZE
#define SET_SIZE (5000000)
#endif
#ifndef TARGET_FPR
#define TARGET_FPR (1)
#endif
#ifndef LOAD_FACTOR
#define LOAD_FACTOR (0.6)
#endif
#ifndef FINGERPRINT_WIDTH
#define FINGERPRINT_WIDTH (12)
#endif
// ------------------------------------------

// Determine the type for fingerprints and indices based on constants
// A 12-bit fingerprint can be stored in a uint16_t (or uint8_t if careful masking is used)
using FingerprintT = uint16_t;

// The mask to extract the FP from the full hash value
const uint64_t FINGERPRINT_MASK = (1ULL << FINGERPRINT_WIDTH) - 1;

/**
 * @brief A macro for the hash function to make it more clear in the code.
 * This should not be used outside the module as some parameters are very specific
 * to the implementation of the class
 * @param data A pointer to the data to hash
 */
#define HASH_VALUE(data) (XXH64(data, sizeof(KeyT), 0))

template <class KeyT>
class XORFilter{
public:
    /**
     * @brief Constructs the XOR filter from a set of keys.
     * @param keys A constant reference to the vector of keys to insert.
     */
    XORFilter(const std::vector<KeyT>& keys);
    
    /**
     * @brief Destructor.
     */
    ~XORFilter();

    /**
     * @brief Checks if a key exists in the filter.
     * @param key The key to check.
     * @return true if the key is likely present, false otherwise.
     */
    bool exist(KeyT key) const;

private:
    // --- Configuration Parameters ---
    size_t M; // Total number of slots (M = ceil(N / LOAD_FACTOR))
    size_t N; // Number of keys (size of the input set)
    size_t B; // Number of blocks (e.g., 3 for a tripartite construction)

    // The backing array for the fingerprints (the actual filter)
    std::vector<FingerprintT> table;

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
     * @brief Calculates the three potential indices (slots) for a key.
     * @param full_hash The full 64-bit hash of the key.
     * @param h The block index for the key (0, 1, or 2).
     * @param i1 Output for index 1.
     * @param i2 Output for index 2.
     * @param i3 Output for index 3.
     */
    void get_indices(uint64_t full_hash, uint8_t h, size_t& i1, size_t& i2, size_t& i3) const {
        // Use three different parts of the full_hash for indices
        // A common technique: use h=0 for block 0, h=1 for block 1, etc.
        
        // This is a simplified block assignment (typically more robust hash mixing is used)
        size_t block_size = M / B; // Size of a single block
        
        // Use a mix of the full_hash and the block index 'h' to get indices in each block
        // Block 0:
        i1 = (uint32_t)(full_hash) % block_size; 
        // Block 1:
        i2 = (uint32_t)(full_hash >> 16) % block_size + block_size; 
        // Block 2:
        i3 = (uint32_t)(full_hash >> 32) % block_size + 2 * block_size;
        
        // For a more robust XOR filter, a two-choice (2 blocks) or 
        // three-choice (3 blocks) scheme is used, where the three indices
        // i1, i2, i3 are computed deterministically. The total number of slots M
        // is distributed across B blocks (e.g., M/B slots per block).
        // The indices must be calculated such that:
        // i1 is in block 0
        // i2 is in block 1
        // i3 is in block 2
        // A common formula is:
        // i_k = (hash_k(full_hash) + offset_k) % block_size + block_start
    }
};

// --- Implementation Details ---

template <class KeyT>
XORFilter<KeyT>::XORFilter(const std::vector<KeyT>& keys) 
    : N(keys.size()) {

    // 1. Determine Parameters
    if (N == 0) {
        M = 0;
        B = 0;
        return;
    }
    
    // M: Total size (number of slots)
    M = std::ceil(N / LOAD_FACTOR);
    // B: Number of blocks (typically 3 for the basic XOR filter)
    B = 3; 

    // Adjust M to be a multiple of B for simple block assignment (optional but common)
    M = std::ceil(M / (double)B) * B;
    
    table.resize(M, 0); // Initialize table of M slots
    
    // --- 2. and 3. Assignment (Graph Peeling/Mapping) ---
    // This is the most complex part. It involves:
    // a) Mapping all N keys to their three potential indices (i1, i2, i3).
    // b) Building an assignment graph (keys as vertices on one side, slots as vertices on the other).
    // c) Peeling: Finding vertices (slots) with degree 1, assigning the key to that slot, 
    //    and removing them and their associated key from the graph (and its edges).
    // d) Storing the resulting assignment order.
    
    // PSEUDOCODE FOR PEELING (Assignment)
    /*
    std::vector<uint64_t> key_hashes(N);
    std::vector<std::array<size_t, 3>> key_indices(N);
    std::vector<std::vector<size_t>> slot_to_key_map(M); // Stores keys associated with each slot

    // 1. Initial Mapping and Graph Construction
    for (size_t i = 0; i < N; ++i) {
        // Calculate hash and indices for keys[i]
        uint64_t full_hash;
        get_raw_fingerprint(keys[i], full_hash);
        key_hashes[i] = full_hash;
        get_indices(full_hash, 0, key_indices[i][0], key_indices[i][1], key_indices[i][2]);
        
        // Add edges to the graph structure (slot_to_key_map)
        for (int j = 0; j < 3; ++j) {
            slot_to_key_map[key_indices[i][j]].push_back(i);
        }
    }

    // 2. Peeling (Finding the assignment order)
    std::vector<std::pair<size_t, size_t>> assignment_stack; // pair: {key_index, slot_index}
    std::queue<size_t> peel_queue; // Slots with degree 1
    std::vector<size_t> slot_degree(M); // Degree of each slot
    // (Initialize slot_degree and peel_queue)

    while (!peel_queue.empty()) {
        // Get a slot with degree 1
        size_t v_idx = peel_queue.front();
        peel_queue.pop();
        
        // Find the key 'k' assigned to this slot
        size_t k_idx = slot_to_key_map[v_idx][0]; // Only one key remains

        // Record the assignment
        assignment_stack.push_back({k_idx, v_idx}); 

        // "Remove" the key 'k' and update degrees of its other two slots
        // ... (This involves complex index tracking and degree updates)
    }

    // Check if the graph was successfully peeled (assignment_stack.size() == N)
    if (assignment_stack.size() != N) {
        // CONSTRUCTION FAILED: Need to re-run with new seeds/hashes. 
        // For a full implementation, you'd loop this entire process with new hash seeds.
        throw std::runtime_error("XOR Filter construction failed. Re-run with new seeds.");
    }
    
    // --- 4. Solving (Calculating Fingerprints) ---
    // The assignment stack is processed in reverse (from last peeled to first peeled).
    // The fingerprints are determined by XORing the raw fingerprint with the 
    // fingerprints already assigned to the other two slots.
    
    for (int i = assignment_stack.size() - 1; i >= 0; --i) {
        size_t k_idx = assignment_stack[i].first;
        size_t assigned_v_idx = assignment_stack[i].second;

        uint64_t full_hash = key_hashes[k_idx];
        FingerprintT raw_fp = get_raw_fingerprint(keys[k_idx], full_hash);
        
        // Get the three potential indices (i1, i2, i3) for the key k_idx
        size_t i1, i2, i3;
        get_indices(full_hash, 0, i1, i2, i3); 

        // The value for the assigned slot is calculated by XORing the raw FP
        // with the fingerprints of the other two slots.
        FingerprintT xor_sum = raw_fp;
        
        if (assigned_v_idx != i1) xor_sum ^= table[i1];
        if (assigned_v_idx != i2) xor_sum ^= table[i2];
        if (assigned_v_idx != i3) xor_sum ^= table[i3];

        // Assign the calculated fingerprint to the assigned slot
        table[assigned_v_idx] = xor_sum;
    }
    */
    // END OF PSEUDOCODE
}

template <class KeyT>
XORFilter<KeyT>::~XORFilter() {
    // Nothing complex to clean up, std::vector handles memory
}

template <class KeyT>
bool XORFilter<KeyT>::exist(KeyT key) const {
    if (M == 0) return false;

    uint64_t full_hash;
    // Get the raw fingerprint and the full hash value
    FingerprintT raw_fp = get_raw_fingerprint(key, full_hash);
    
    // Get the three potential indices
    size_t i1, i2, i3;
    get_indices(full_hash, 0, i1, i2, i3);
    
    // The key is likely present if the raw fingerprint is equal to 
    // the XOR sum of the three stored fingerprints
    
    // Calculate the XOR sum of the three stored fingerprints
    FingerprintT calculated_fp_sum = table[i1] ^ table[i2] ^ table[i3];
    
    // The check: Raw FP == XOR Sum of stored FPs
    return raw_fp == calculated_fp_sum;
}

#endif //XOR_FILTER_H