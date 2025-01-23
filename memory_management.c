/*
 * File: memory_management.c
 * Description: A memory manager struct holding stacks of pointers to be freed in case of error and 
 * implementations of associated methods.
 * Author: Dor Fuchs
 */

# include <stdio.h>
# include <stdlib.h>
# include <stddef.h>
# include "memory_management.h"
# include "symnmf_ops.h"

/*Function declaration*/
void** initialize_stack(Memory_manager*);
void add_pointer_to_stack(Memory_manager*,StackType, void*);
void** resize_stack(Memory_manager*, void**, int*);
int find_pointer_in_stack(void **, void*, int);
void free_stack(void **, int*);
void free_manager(Memory_manager*);

void initialize_manager(Memory_manager **manager){
    /*Variable declaration*/
    int i;

    /*Memory allocation*/
    (*manager) = malloc(sizeof(Memory_manager));
    for (i = 0; i < NUMBER_OF_STACKS; i++){
        (*manager)->allocations_stack[i] = initialize_stack((*manager));
        (*manager)->current_amount_of_allocations[i] = 0;
        (*manager)->current_stack_sizes[i] = 500;
    }
    (*manager)->current_file = NULL;
}

void* safe_malloc(Memory_manager *manager,size_t size_of_type, StackType stack_type){
    /*Variable declarations*/
    void *output_pointer;
    
    /*Memory allocation*/
    output_pointer = malloc(size_of_type);
    allocation_error_check_and_exit(manager ,output_pointer);

    add_pointer_to_stack(manager, stack_type, output_pointer);
    return output_pointer;
}

void* safe_realloc(Memory_manager *manager, void* pointer_to_realloc, size_t size_of_reallocation, StackType stack_type){
    /*Variable declaration*/
    int previous_position;
    void **current_stack;
    void *previous_pointer;

    /*Memory allocation*/
    previous_pointer = pointer_to_realloc;
    pointer_to_realloc = realloc(pointer_to_realloc, size_of_reallocation);
    allocation_error_check_and_exit(manager ,pointer_to_realloc);

    current_stack = manager->allocations_stack[stack_type];
    previous_position = find_pointer_in_stack(current_stack, previous_pointer, manager->current_amount_of_allocations[stack_type]);
    if (previous_position == -1){
        printf("An Error Has Occurred");
        free_all_memory(manager, 1);
        exit(EXIT_FAILURE);
    }
    current_stack[previous_position] = pointer_to_realloc;
    return pointer_to_realloc;
}

void free_all_memory(Memory_manager *manager, int is_final){
    /*Variable declaration*/
    int i;
    
    /*Free void pointer stacks sequentially*/
    for (i = 0; i < NUMBER_OF_STACKS; i++){
        free_stack(manager->allocations_stack[i], &(manager->current_amount_of_allocations[i]));
    }

    /*Separately handle the file pointer*/
    if(manager->current_file != NULL){
        fclose(manager->current_file);
    }

    /*Reset or free the current manager*/
    if (is_final){
        free_manager(manager);
    }
    else{
        initialize_manager(&manager);
    }
}

void allocation_error_check_and_exit(Memory_manager *manager, void *pointer){
    if (pointer == NULL){
        printf("An Error Has Occurred");
        free_all_memory(manager, 1);
        exit(EXIT_FAILURE);
    }
}

Matrix* safe_initialize_matrix(Memory_manager *manager, int rows, int columns){
    /*Variable declaration*/
    int i;
    Matrix *return_matrix;

    /*Memory allocation*/
    return_matrix = safe_malloc(manager, sizeof(Matrix), STRUCT_STACK);
    return_matrix->data = safe_malloc(manager, rows*sizeof(double*), 2);
    for (i = 0; i < rows; i++){
        return_matrix->data[i] = safe_malloc(manager, columns*sizeof(double), 1);
    }

    return_matrix->rows = rows;
    return_matrix->columns = columns;
    return return_matrix;
}

/*
 * Function: initialize_stack
 * --------------------------
 * Initializes a stack of size 500 void pointers (initially NULL) for a given memory manager and returns it.
 *
 * Parameters:
 *    manager: A pointer to the memory manager to which the stack should be added.
 *    It is necassary since it might not be empty and a faild allocation would 
 *    require a flush of the memory it holds.
 *    
 * Returns:
 *    The new stack with an initial size of 500 void pointers(NULL).
 */
void** initialize_stack(Memory_manager *manager){
    /*Variable declarations*/
    int i;
    void **output_stack;
    
    /*Memory allocation*/
    output_stack = malloc(500*sizeof(void*));
    allocation_error_check_and_exit(manager, output_stack);

    for (i = 0; i < 500; i++) {
        output_stack[i] = NULL;
    }
    return output_stack;
}

/*
 * Function: add_pointer_to_stack
 * --------------------------
 * Adds the given pointer to the given stack and adjusts its size.
 *
 * Parameters:
 *    manager: A pointer to the memory manager which holds the stack to which the pointer
 *    is to be added.
 *    stack_type: The type of the stack to which the pointer should be added.
 *    pointer_to_add: The pointer to be added.
 *    
 * Returns:
 *    Nothing.
 */
void add_pointer_to_stack(Memory_manager *manager,StackType stack_type, void* pointer_to_add){
    /*Variable declaration*/
    int *current_stack_size;
    int *current_stack_max_size;

    current_stack_size = &(manager->current_amount_of_allocations[stack_type]);
    current_stack_max_size = &(manager->current_stack_sizes[stack_type]);
    if ((*current_stack_size) == (*current_stack_max_size)){
        manager->allocations_stack[stack_type] = resize_stack(manager, 
        manager->allocations_stack[stack_type], current_stack_max_size);
    }
    manager->allocations_stack[stack_type][(*current_stack_size)] = pointer_to_add;
    (*current_stack_size)++;
}

/*
 * Function: resize_stack
 * --------------------------
 * Resizes a given stack to twice its max size and sets its maxsize to the new value.
 *
 * Parameters:
 *    manager: A pointer to the memory manager to which the stack should be added.
 *    It is necassary since it might not be empty and a faild allocation would 
 *    require a flush of the memory it holds.
 *    stack_to_resize: The stack that needs resizing.
 *    max_stack_size: A pointer to the maximum size field of the given stack.
 *    
 * Returns:
 *    The pointer to the new block of memory allocated for the stack.
 */
void** resize_stack(Memory_manager *manager, void**stack_to_resize, int *max_stack_size){
    /*Variable declaration*/
    void **new_stack;

    /*Memory allocation*/
    new_stack = realloc(stack_to_resize, 2*(*max_stack_size)*sizeof(void*));
    allocation_error_check_and_exit(manager, new_stack);

    (*max_stack_size) *= 2;
    return new_stack;
}

/*
 * Function: find_pointer_in_stack
 * --------------------------
 * Finds the given pointer in the given stack if possible.
 *
 * Parameters:
 *    stack: A pointer to the stack in which the pointer should be.
 *    pointer_to_find: The pointer to be found in the stack.
 *    stack_size: The corresponding stacks size field.
 *    
 * Returns:
 *    The location of the pointer if found, else -1.
 */
int find_pointer_in_stack(void **stack, void* pointer_to_find, int stack_size){
    /*Variable declaration*/
    int i;

    for (i = 0; i < stack_size; i++){
        if (stack[i] == pointer_to_find){
            return i;
        }
    }
    return -1;
}

/*
 * Function: free_stack
 * --------------------------
 * Frees the entire manager stack given and sets its size to 0;
 *
 * Parameters:
 *    stack: A pointer to the stack to be freed.
 *    stack_size: A pointer to the corresponding stacks size field in the manager.
 *    
 * Returns:
 *    Nothing.
 */
void free_stack(void **stack, int *stack_size){
    /*Variable declaration*/
    int i;
    void *current_pointer;

    for (i = (*stack_size) - 1; i >= 0; i--){
        current_pointer = stack[i];
        if (current_pointer != NULL){
            free(current_pointer);
            stack[i] = NULL;
        }
    }
    (*stack_size) = 0;
}

/*
 * Function: free_manager
 * --------------------------
 * Frees the memory manager struct and its dynamically allocated stacks.
 *
 * Parameters:
 *    manager: A pointer to the memory manager to be freed.
 *    
 * Returns:
 *    Nothing.
 */
void free_manager(Memory_manager *manager){
    /*Variable declaration*/
    int i;
    void **current_stack;

    for (i = 0; i < NUMBER_OF_STACKS; i++){
        current_stack = manager->allocations_stack[i];
        if (current_stack != NULL){
            free(current_stack);
            manager->allocations_stack[i] = NULL;
        }
    }
    free(manager);
}
