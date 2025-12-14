//
// Created by Alek on 11/16/2025.
//
#include <iostream>

#ifndef CONCURRENT_HASH_TABLE_H
#define CONCURRENT_HASH_TABLE_H

#include <shared_mutex>
#include "xxhash.h"
#include <iostream>
#include <mutex>
#include <syncstream>
#include <thread>
#include <chrono>
#include <utility>

/**
 * @brief A macro for the hash function to make it more clear in the code.
 *     This should not be used outside the module as some parameters are very specific
 *     to the implementation of the class
 * @param data A pointer to the data to hash
 */
#define GET_TABLE_LOC(data) (XXH64(data, sizeof(KeyT), hashseed)%table_size)
#define GET_NEW_TABLE_LOC(data) (XXH64(data, sizeof(KeyT), hashseed)%new_size)


template <class KeyT, class ValT>
struct hash_table_kv_pair_t{
    KeyT key;
    ValT val;
    hash_table_kv_pair_t<KeyT, ValT> * next;
};

template <class KeyT, class ValT>
struct hash_table_kv_pair_head_t
{
    hash_table_kv_pair_t<KeyT, ValT> kv_pair;
    bool valid;
#ifdef FINE_GRAINED
    std::shared_mutex * mutex;
#endif
};

#define RESIZE_RATE (0.5)

template <class KeyT, class ValT>
class ConcurrentHashTable
{
public:
    ConcurrentHashTable(unsigned long long starting_size);
    bool insert(KeyT key, ValT value);
    bool remove(KeyT key);

    void visualize(std::ostream& os);

    std::pair<bool, ValT> find(KeyT key);

private:
    hash_table_kv_pair_head_t<KeyT, ValT> * table;


    /**
     * @brief A lock for this data structure to ensure that writes cannot occur when
     *     reading is also occurring.
     * @note This variable will be used even in fined grained locking mode for
     *    when resizing occurs at which point nothing should attempt to RW the structure
     */
    std::shared_mutex global_mutex;

    unsigned long long num_elements = 0;
    unsigned long long table_size;

    /**
     * @brief The seed to use for the hash function.
     * @note This seed is set to a random value of construction of the class
     *     but hashing is already fairly random so no need to make it super
     *     complex.  However, it is nice to hash slightly differently
     *     on different runs of the program incase a particular
     *     seed results in a particularly bad hash function for numbers.
     */
    unsigned int hashseed;

    void double_size(unsigned long long target);
};


/**
 *===============================================================================
 * CONSTRUCTOR DEFINTIONS
 *===============================================================================
*/

template<class KeyT, class ValT>
ConcurrentHashTable<KeyT, ValT>::ConcurrentHashTable(unsigned long long starting_size)
{
    table_size = starting_size;
    table = new hash_table_kv_pair_head_t<KeyT, ValT>[table_size];

    // Make sure all locations are invalid to start
    for (unsigned long long i = 0; i<table_size; ++i)
    {
        table[i].valid = false;
#ifdef FINE_GRAINED
        table[i].mutex = new std::shared_mutex;
#endif
    }
    hashseed = std::time(nullptr);
}



template <class KeyT, class ValT>
bool ConcurrentHashTable<KeyT, ValT>::insert(KeyT key, ValT value)
{

    bool already_exists = false;

    unsigned long long curr_table_size = table_size;
    // Check if we need to resize
    if (num_elements+1 >= curr_table_size*RESIZE_RATE)
    {
        double_size(curr_table_size<<1);
    }

#ifdef FINE_GRAINED
    // For fine-grained we take a non-unique global lock to prevent resizing
    // but allow for other operations
    std::shared_lock<std::shared_mutex> lock_global_ops(global_mutex);
#endif
#ifndef FINE_GRAINED
    // Take the global lock for course grained
    std::unique_lock<std::shared_mutex> lock_global_ops(global_mutex);
#endif

    // Need to lock before hashing incase of resize

    unsigned long long loc = GET_TABLE_LOC(&key);
#if FINE_GRAINED
    // If its fine-grained only take the specific lock for writing
    std::unique_lock<std::shared_mutex> local_lock(*table[loc].mutex);
#endif

    if (table[loc].valid == false)
    {
        table[loc].valid = true;
        table[loc].kv_pair.key = key;
        table[loc].kv_pair.val = value;
        table[loc].kv_pair.next = nullptr;
        ++num_elements;
    }
    else
    {
        hash_table_kv_pair_t<KeyT, ValT> * current_node = &table[loc].kv_pair;
        while (current_node->next != nullptr)
        {
            if (key == current_node->key)
            {
                // At this point we found the key already exists in the table so dont
                //     add it again
                already_exists = true;
                break;
            }
            current_node = current_node->next;
        }
        if (key == current_node->key) already_exists = true;
        // At this point if we have found anything we need to create a new node in the list
        if (already_exists == false)
        {
            hash_table_kv_pair_t<KeyT, ValT> * next_node = new hash_table_kv_pair_t<KeyT, ValT>;
            next_node->key = key;
            next_node->val = value;
            next_node->next = nullptr;
            current_node->next = next_node;
            ++num_elements;
        }
    }
    // If we didn't already return then we added the node so return true
    return !already_exists;
}

template <class KeyT, class ValT>
std::pair<bool, ValT> ConcurrentHashTable<KeyT, ValT>::find(KeyT key)
{
    std::pair<bool, ValT> retval = std::make_pair(false, ValT());

    // Always lock the global one.
    // In fine grained mode we need the shared lock for global to prevent
    //     resizing and for course grained the shared lock is needed for
    //     preventing writes / resizing of the entire structure
    std::shared_lock<std::shared_mutex> lock_global_ops(global_mutex);

    unsigned long long loc = GET_TABLE_LOC(&key);

#if FINE_GRAINED
    // If fine-grained locking is enabled then we also need to grab the lock for
    //     that individual index
    std::shared_lock<std::shared_mutex> local_lock(*table[loc].mutex);
#endif

    // Return false if the value isnt found
    if (table[loc].valid == false)
    {

    }
    else
    {
        hash_table_kv_pair_t<KeyT, ValT> * current_node = &table[loc].kv_pair;
        while (current_node->next != nullptr)
        {
            if (key == current_node->key)
            {
                break;
            }
            current_node = current_node->next;
        }
        if (key == current_node->key)
        {
            retval.second = current_node->val;
            retval.first = true;
        }
    }
#ifdef FINE_GRAINED
#endif

#ifndef FINE_GRAINED

#endif

    return retval;
}

template <class KeyT, class ValT>
void ConcurrentHashTable<KeyT, ValT>::visualize(std::ostream& os)
{
    std::shared_lock<std::shared_mutex> lock_global_ops(global_mutex);

    for (unsigned int i = 0; i < table_size; ++i)
    {
        if (table[i].valid == true)
        {
            os << "[" << i << "] : ";
            hash_table_kv_pair_t<KeyT, ValT> * curr_node = &table[i].kv_pair;
            while (curr_node->next != nullptr)
            {
                os << "{" << curr_node->key << ", " << curr_node->val << "}" << " -> ";
                curr_node = curr_node->next;
            }
            os << "{" << curr_node->key << ", " << curr_node->val << "}" << std::endl;
        }
    }
}


/**
 *===================================================================
 * Private HELPER FUNCTIONS
 *===================================================================
 */


template <class KeyT, class ValT>
void ConcurrentHashTable<KeyT, ValT>::double_size(unsigned long long target_size)
{
    // Take the global lock even if we are in fine-grained mode.  Cant trust the resizing
    //    process to not interfere with other threads.
    std::unique_lock<std::shared_mutex> lock_global_ops(global_mutex);

    // This is really important as a bunch of threads can in theroy all try to resize at the same
    //     time.. this should not be allowed to happen so if a resize has occured since the function
    //     was called then just let go of the lock and
    if (table_size >= target_size)
    {
        return;
    }

    // We absoultely need to lock before determining the new size as otherwise two threads trying to
    //     resize the table could do so incorrectly

    // Just double the table size
    unsigned long long new_size = table_size << 1;;

    hash_table_kv_pair_head_t<KeyT, ValT> * newtable = new hash_table_kv_pair_head_t<KeyT, ValT> [new_size];

    // Start by making all nodes invalid
    for (unsigned long long i = 0; i<new_size; ++i)
    {
        newtable[i].valid = false;
#ifdef FINE_GRAINED
        newtable[i].mutex = new std::shared_mutex();
#endif
    }

    for (unsigned long long i = 0; i<table_size; ++i)
    {
        // If valid data at this location then move it to the new table
        if (table[i].valid == true)
        {
            hash_table_kv_pair_t<KeyT, ValT> * current_node = &table[i].kv_pair;

            // Copy all nodes in the chain to their new position
            bool need_to_delete = false;
            while (current_node != nullptr)
            {
                unsigned long long newloc = GET_NEW_TABLE_LOC(&current_node->key);

                if (newtable[newloc].valid == false)
                {
                    newtable[newloc].valid = true;
                    newtable[newloc].kv_pair.key = current_node->key;
                    newtable[newloc].kv_pair.val = current_node->val;
                    newtable[newloc].kv_pair.next = nullptr;
                }
                else
                {
                    // If somethings already there,
                    hash_table_kv_pair_t<KeyT, ValT> * newnode = new hash_table_kv_pair_t<KeyT, ValT>;
                    newnode->key = current_node->key;
                    newnode->val = current_node->val;
                    // Reassign the new node
                    newnode->next = newtable[newloc].kv_pair.next;
                    newtable[newloc].kv_pair.next = newnode;
                }
                hash_table_kv_pair_t<KeyT, ValT> * next_node = current_node->next;
                if (need_to_delete == false) need_to_delete = true;
                else delete current_node;
                current_node = next_node;
            }
        }
#ifdef FINE_GRAINED
        // If we are fine grained then we need to delete the old mutex
        delete table[i].mutex;
#endif
    }
    // Clean up the old table and set to the correct size
    table_size = new_size;
    delete [] table;
    table = newtable;

}
#endif //CONCURRENT_HASH_TABLE_H


