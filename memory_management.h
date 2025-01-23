/*
 * File: memory_management.h
 * Description: Header file for the memory manager struct and its associated functions.
 * Author: Dor Fuchs
 */
# ifndef MEMORY_MANAGEMENT_H
# define MEMORY_MANAGEMENT_H
# include <stddef.h>
# include "matrix.h"

/*Constant defenition*/
# define NUMBER_OF_STACKS 3
# define INITIAL_STACK_SIZE 500

/*Enum definition*/
typedef enum {
    ONE_DIMENSION = 0,
    TWO_DIMENSIONS = 1,
    STRUCT_STACK = 2
} StackType;

/*Struct definition*/
typedef struct {
    void **allocations_stack[NUMBER_OF_STACKS];
    int current_amount_of_allocations[NUMBER_OF_STACKS];
    int current_stack_sizes[NUMBER_OF_STACKS];
    FILE *current_file;
} Memory_manager;


/*Function declaration*/

/*
 * Function: initialize_manager
 * --------------------------
 * Sets a given memory managers stack sizes to 0.
 *
 * Parameters:
 *    manager: A pointer to the memory manager instance that tracks allocations.
 *    
 * Returns:
 *    Nothing.
 */
void initialize_manager(Memory_manager **manager);

/*
 * Function: safe_malloc
 * --------------------------
 * Allocates memory and registers the pointer with the memory manager, allowing
 * for safe freeing in case of errors.
 *
 * Parameters:
 *    manager: A pointer to the memory manager instance that tracks allocations.
 *    size_of_type: The size of the requested memory block in bytes.
 *    stack_type: Indicates the type of memory allocation:
 *               ONE_DIMENSION - One-dimensional arrays
 *               TWO_DIMENSIONS - Two-dimensional arrays
 *               STRUCT - Structs
 *    
 * Returns:
 *    A void pointer to the allocated memory block of the requested size.
 */
void* safe_malloc(Memory_manager *manager,size_t size_of_type, StackType stack_type);

/*
 * Function: safe_realloc
 * --------------------------
 * Safely reallocates memory by replacing the corresponding pointer in the memory manager with the new one 
 * and flushing it out in case of a memory allocation error.
 *
 * Parameters:
 *    manager: A pointer to the memory manager instance that tracks allocations.
 *    pointer_to_realloc: A void type pointer to be reallocated to a new place in memory.
 *    size_of_reallocation: The requested size of the new memory block.
 *    stack_type: The dimension of the array pointed to, indicating the stack holding the pointer to be reallocated.
*                  1 - One dimensional arrays
*                  2 - Two dimensional arrays
*    
 * Returns:
 *    The pointer to the new block of memory.
 */
void* safe_realloc(Memory_manager *manager, void* pointer_to_realloc, size_t size_of_reallocation, StackType stack_type);

/*
 * Function: add_file_pointer_to_stack
 * --------------------------
 * Adds the given file pointer to the managers file pointer stack and adjusts its size.
 *
 * Parameters:
 *    stack: A pointer to the managers file pointer stack.
 *    pointer_to_add: The file pointer to be added to the stack.
 *    stack_size: A pointer to the corresponding stacks size field in the manager.
 *    
 * Returns:
 *    Nothing.
 */
void add_file_pointer_to_stack(FILE **stack, FILE* pointer_to_add, int *stack_size);

/*
 * Function: free_all_memory
 * --------------------------
 * Frees all memory in the pointer stacks at the end of the program or when encountering an error.
 *
 * Parameters:
 *    manager: A pointer to the memory manager instance that tracks allocations.
 *    is_final: Set to 1 if the program is ending, to also free the memory manager instance itself.
 *    
 * Returns:
 *    Nothing.
 */
void free_all_memory(Memory_manager *manager, int is_final);

/*
 * Function: allocation_error_check_and_exit
 * --------------------------
 * Checks for a memory allocation error and if found, prints an error message, flushes the memory and exits the program.
 *
 * Parameters:
 *    manager: A pointer to the memory manager instance that tracks allocations.
 *    
 * Returns:
 *    Nothing.
 */
void allocation_error_check_and_exit(Memory_manager *manager, void *pointer);

/*
 * Function: safe_initialize_matrix
 * --------------------------
 * Allocate memory for a matrix struct and set its rows and columns fields to the requested amount using safe_malloc.
 *
 * Parameters:
 *    manager: manager: A pointer to the memory manager instance that tracks allocations.
 *    rows: Requested amount of rows.
 *    columns: Requested amount of columns.
 * 
 * Returns:
 *    A pointer to the initialized matrix.
 */
Matrix* safe_initialize_matrix(Memory_manager *manager, int rows, int columns);
#endif
