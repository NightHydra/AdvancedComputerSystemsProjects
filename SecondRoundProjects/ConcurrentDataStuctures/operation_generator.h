//
// Created by Alek on 12/13/2025.
//
#include <random>

#ifndef OPERATION_GENERATOR_H
#define OPERATION_GENERATOR_H

#ifndef SUCCESSFUL_FIND_RATE
#define SUCCESSFUL_FIND_RATE (0.5)
#endif

typedef enum
{
   FIND,
   INSERT
} operation_type_t;

template <class KeyT, class ValT>
struct operation_t
{
   operation_type_t operation_type;
   KeyT key;
   /**
    * @brief This field is the expected value upon find and the value to insert
    *    when this struct represents and insert operation
    */
   ValT value;

   /**
    * The below values are booleans representing whether a find successfully found what it
    *     was looking for
    */
   bool expected_result_bool;
   bool actual_result_bool;
};

template <class KeyT, class ValT>
class OperationGenerator {

public:
   OperationGenerator(int prefilled_keys_len, KeyT const * const initial_keys);

   operation_t<KeyT, ValT> generate_insert_operation();
   operation_t<KeyT, ValT> generate_find_operation();

private:

   std::random_device rd;
   std::mt19937 gen;

   std::uniform_int_distribution<KeyT> key_generator;
   std::uniform_int_distribution<ValT> value_generator;
   std::uniform_int_distribution<int> index_selection_generator;
   std::bernoulli_distribution valid_find_generator;

   KeyT const * prefilled_keys;
};


// Function Definitions
template<class KeyT, class ValT>
OperationGenerator<KeyT, ValT>::OperationGenerator(int prefilled_keys_len, KeyT const * const initial_keys) :
    gen(rd()), key_generator(0, std::numeric_limits<int>::max()), value_generator(0, std::numeric_limits<int>::max()),
    index_selection_generator(0, 0), valid_find_generator(SUCCESSFUL_FIND_RATE)
{
   prefilled_keys = initial_keys;
}

template<class KeyT, class ValT>
operation_t<KeyT, ValT> OperationGenerator<KeyT, ValT>::generate_insert_operation()
{
   operation_t<KeyT, ValT> operation;
   operation.key = key_generator(gen);
   operation.value = value_generator(gen);
   operation.operation_type = INSERT;
   return operation;
}

template <class KeyT, class ValT>
operation_t<KeyT, ValT> OperationGenerator<KeyT, ValT>::generate_find_operation()
{
   // If valid then take a previous key and use it
   operation_t<KeyT, ValT> operation;
   operation.operation_type = FIND;

   if (valid_find_generator(gen) == true)
   {
      operation.key = prefilled_keys[index_selection_generator(gen)];
      //std::cout << operation.key << ' ';
      operation.expected_result_bool = true;
   }
   else
   {
      operation.key = key_generator(gen);
      operation.expected_result_bool = false;
   }
   return operation;
}



#endif //OPERATION_GENERATOR_H
