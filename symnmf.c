/*
 * File: symnmf.c
 * Description: Implementation of the Symmetric Non Negative Matrix Factorization algorithm.
 * and some related sub-methods
 * Author: Dor Fuchs
 */

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>
# include <stddef.h>
# include "symnmf.h"
# include "symnmf_ops.h"
# include "memory_management.h"

/*Constants definition*/
#define MAX_PATH_SIZE 100
#define MAX_GOAL_SIZE 4
#define ASSUMED_AMOUNT_OF_LINES 10
#define ASSUMED_LINE_SIZE 64
#define MAX_ITERATION_NUMBER 300
#define EPSILON 0.0001 

/*Function declaration*/
void execute_goal_and_print(Matrix *, int);
Matrix* parse_input_file(char*);
size_t read_line(char**, size_t*, FILE*);
Matrix* transform_raw_data(char **, int);
int find_dimension(char *);
void print_matrix(double**, int, int);

/*Global variable declaration*/
Memory_manager *symnmf_manager;

Matrix* sym(Memory_manager *module_manager, Matrix *data_points, int for_python){
    /*Variable declaration*/
    int i;
    int j;
    int amount_of_points = data_points->rows;
    int dimension = data_points->columns;
    double current_similarity_value;
    Matrix *similarity_matrix;


    /*Memory allocation*/
    if (for_python){
        similarity_matrix = safe_initialize_matrix(module_manager,amount_of_points, amount_of_points);
    }
    else{
        similarity_matrix = safe_initialize_matrix(symnmf_manager,amount_of_points, amount_of_points);
    }

    for (i = 0; i < amount_of_points; i++){
        similarity_matrix->data[i][i] = 0;
        for (j = i + 1; j < amount_of_points; j++){
            current_similarity_value = calc_similarity(data_points->data[i], data_points->data[j], dimension);
            similarity_matrix->data[i][j] = current_similarity_value;
            similarity_matrix->data[j][i] = current_similarity_value;
        }
    }
    return similarity_matrix;
}

double* ddg(Memory_manager *module_manager, Matrix *similarity_matrix, Matrix *data_points, int for_python){
    /*Variable declaration*/
    int i;
    int j;
    int amount_of_points = data_points->rows;
    double *diagonal_degree_matrix;

    /*Memory allocation*/
    if (for_python){
        diagonal_degree_matrix = safe_malloc(module_manager,amount_of_points*sizeof(double), 1);
    }
    else{
        diagonal_degree_matrix = safe_malloc(symnmf_manager,amount_of_points*sizeof(double), 1);
    }

    for (i = 0; i < amount_of_points; i++){
        diagonal_degree_matrix[i] = 0;
        for (j = 0; j < amount_of_points; j++){
            diagonal_degree_matrix[i] += similarity_matrix->data[i][j];
        }
    }
    return diagonal_degree_matrix;
}

Matrix* norm(Memory_manager *module_manager, Matrix *similarity_matrix, double *diagonal_degree_matrix, Matrix *data_points, int for_python){
    /*Variable declaration*/
    int i;
    int j;
    int amount_of_points = data_points->rows;
    double diagonal_i;
    Matrix *normal_similarity_matrix;

    /*Memory allocation*/
    if (for_python){
        normal_similarity_matrix = safe_initialize_matrix(module_manager,amount_of_points, amount_of_points);
    }
    else{
        normal_similarity_matrix = safe_initialize_matrix(symnmf_manager,amount_of_points, amount_of_points);
    }

    for (i = 0; i < amount_of_points; i++){
        normal_similarity_matrix->data[i][i] = 0;
        diagonal_i = diagonal_degree_matrix[i];
        for (j = i + 1; j < amount_of_points; j++){
            normal_similarity_matrix->data[i][j] = similarity_matrix->data[i][j]/sqrt(diagonal_i*diagonal_degree_matrix[j]);
            normal_similarity_matrix->data[j][i] = normal_similarity_matrix->data[i][j];
        }
    }
    return normal_similarity_matrix;
}

Matrix *symnmf(Memory_manager *module_manager, Matrix *H, Matrix *W){
    /*Variable declaration*/
    Matrix *H_t_plus_1;
    int i;

    /*Memory allocation*/
    H_t_plus_1 = safe_initialize_matrix(module_manager ,H->rows, H->columns);

    for (i = 0; i < MAX_ITERATION_NUMBER; i++){
        calculate_step(symnmf_manager, H_t_plus_1, H, W);
        if (squared_frobenius_norm_of_difference(H_t_plus_1, H) < EPSILON){
            return H_t_plus_1;
        }
    }
    return H_t_plus_1;
}

/*
 * Function: execute_goal_and_print
 * --------------------------
 * Executes the goal and prints the required matrix.
 * 1 - Calculate and print the similarity matrix.
 * 2 - Calculate the similarity and ddg matrix and print the ddg matrix.
 * 3 - Calculate the similarity, ddg, and normalized similarity matrix and print the latter.
 * 
 * Parameters:
 *    data_points: A Matrix struct holding the data points, their amount(rows) and their dimension(columns)
 *
 * Returns:
 *    Nothing.
 */
void execute_goal_and_print(Matrix *data_points, int goal){
    /*Variable declaration*/
    Matrix *similarity_matrix;
    double *diagonal_degree_matrix;
    Matrix *normalized_similarity_matrix;
    int amount = data_points->rows;

    /*Memory allocation*/
    similarity_matrix = sym(NULL, data_points, 0);
    if (goal == 1){
        print_matrix(similarity_matrix->data, amount, 0);
    }
    else{
        diagonal_degree_matrix = ddg(NULL, similarity_matrix, data_points, 0);
        if (goal == 2){
            /*Since the ddg is a 1-d array and the print_matrix function only accepts 2-d arrays
            *its adress is sent and it is de-referenced accordingly using the diag flag
            */
            print_matrix(&diagonal_degree_matrix, amount, 1);
        }    
        else{
            normalized_similarity_matrix = norm(NULL, similarity_matrix, diagonal_degree_matrix, data_points, 0);
            print_matrix(normalized_similarity_matrix->data, amount, 0);
        }
    }
}

/*
 * Function: parse_input_file
 * --------------------------
 * Extracts the points, their amount and their dimension from the input file.
 *
 * Parameters:
 *    input_file: The file path of the input file.
 *
 * Returns:
 *    A pointer to a Matrix struct holding a two dimensional array of data points, their amount(rows) and their dimension(columns).
 */
Matrix* parse_input_file(char *input_file){
    /*Variable declaration*/
    FILE *input_file_pointer = NULL;
    char *current_line = NULL;
    char **raw_input_data = NULL;
    size_t amount_of_lines = ASSUMED_AMOUNT_OF_LINES;
    size_t current_line_index = 0;
    size_t line_size = ASSUMED_LINE_SIZE;
    size_t number_of_bytes_read;

    /*Memory allocation*/
    raw_input_data = safe_malloc(symnmf_manager, amount_of_lines*sizeof(char*), 2);
    input_file_pointer = fopen(input_file, "r");
    allocation_error_check_and_exit(symnmf_manager, (void*)input_file_pointer);
    symnmf_manager->current_file = input_file_pointer;
    current_line = safe_malloc(symnmf_manager, (line_size)*sizeof(char), 1);
    while((number_of_bytes_read = read_line(&current_line, &line_size, input_file_pointer)) > 0){
        if (current_line_index == amount_of_lines - 1){
            raw_input_data = safe_realloc(symnmf_manager, raw_input_data, 2*amount_of_lines*sizeof(char*), 2);
            amount_of_lines *= 2;
        }
        raw_input_data[current_line_index] = safe_malloc(symnmf_manager, (number_of_bytes_read + 1)*sizeof(char), 1);
        memcpy(raw_input_data[current_line_index], current_line, (number_of_bytes_read + 1));
        current_line_index++;
    }
    fclose(symnmf_manager->current_file);
    symnmf_manager->current_file = NULL;
    return transform_raw_data(raw_input_data, current_line_index);
}

/*
 * Function: read_line
 * --------------------------
 * Reads a line from the input file while dynamically updating the buffer size if needed.
 *
 * Parameters:
 *    current_line: A pointer to the line trying to be read from the file.
 *    line_size A pointer to the integer holding the size of the buffer, updated to the new size if needed.
 *    file_pointer: The pointer pointing to the place currently read from in the file.
 *    
 * Returns:
 *    The size of data read from the file.
 */
size_t read_line(char **current_line, size_t *line_size, FILE *file_pointer){
    /*Variable declaration*/
    size_t length = 0;

    while(fgets(*current_line + length, *line_size - length, file_pointer) != NULL){
        length += strlen(*current_line + length);
        if(((*current_line)[length - 1]) != '\n'){
            (*line_size) *= 2;
            (*current_line) = safe_realloc(symnmf_manager, *current_line, (*line_size), 1);
        }
        else{
            break;
        }
    }
    return length;
}

/*
 * Function: transform_raw_data
 * --------------------------
 * Transforms the raw data, extracted from the file in the form of strings, into an array of doubles.
 *
 * Parameters:
 *    raw_data: A two dimensional string array of the data read from the file.
 *    amount_of_points: The amount of lines read from the file, corresponding to the amount of data points.
 *    
 * Returns:
 *     A pointer to a matrix struct holding the data points, their amount(rows), and their dimension(columns)
 */
Matrix* transform_raw_data(char **raw_data, int amount_of_points){
    /*Variable declaration*/
    Matrix *data_points;
    int i;
    int j;
    int dimension = find_dimension(raw_data[0]);
    char *tokenized_point;

    /*Memory allocation*/
    data_points = safe_initialize_matrix(symnmf_manager, amount_of_points, dimension);

    for (i = 0; i < amount_of_points; i++){
        tokenized_point = strtok(raw_data[i], ",");
        for (j=0; j<dimension; j++){
            allocation_error_check_and_exit(symnmf_manager, tokenized_point);
            data_points->data[i][j] = strtod(tokenized_point, NULL);
            tokenized_point = strtok(NULL, ",");
        }
    }
    return data_points;
}

/*
 * Function: find_dimension
 * --------------------------
 * Determines the dimension of the data points by counting the number of commas
 * in the input line, plus one.
 *
 * Parameters:
 *    raw_point: A data point in its string form.
 *    
 * Returns:
 *    An integer representing the dimension of the data points.
 */
int find_dimension(char *raw_point){
    /*Variable declaration*/
    int dimension = 0;
    int raw_length = strlen(raw_point);
    int i;

    for (i = 0; i < raw_length; i++){
        if (raw_point[i] == ','){
            dimension++;
        }
    }
    return dimension+1;   
}

/*
 * Function: print_matrix
 * --------------------------
 * Prints the elements of a given square matrix with precision of 4 digits after the decimal point, 
 * a comma between values and a new line after each row has been printed.
 *
 * Parameters:
 *    matrix: The matrix to be printed.
 *    dimension: The matrix' dimension.
 *    is_diag: Flag to indicate that the matrix is diagonal.
 *
 * Returns:
 *    Nothing.
 */
void print_matrix(double** matrix, int dimension, int is_diag){
    /*Varaible declaration*/
    int i;
    int j;

    if (!is_diag){
        for (i = 0; i < dimension; i++){
            for (j = 0; j < dimension; j++){
                printf("%.4f", matrix[i][j]);
                if (j < dimension - 1){
                    printf(",");
                }
            }
            printf("\n");
        }
    }
    else{
        for (i = 0; i < dimension; i++){
            for (j = 0; j < dimension; j++){
                if (i == j){
                    printf("%.4f", (*matrix)[i]);
                }
                else{
                    printf("%.4f", 0.0);
                }
                if (j < dimension - 1){
                    printf(",");
                }
            }
            printf("\n");
        }
    }
}

int main(int argc, char *argv[]){
    /*Variable declaration*/
    char *goal = NULL;
    char *input_file = NULL;
    Matrix *data_points;

    /*Memory allocation*/
    initialize_manager(&symnmf_manager);
    goal = safe_malloc(symnmf_manager ,MAX_GOAL_SIZE*sizeof(char), 1);
    input_file = safe_malloc(symnmf_manager, MAX_PATH_SIZE*sizeof(char), 1);
    if(argc != 3){
        allocation_error_check_and_exit(symnmf_manager, NULL);
    }
    strncpy(goal, argv[1], MAX_GOAL_SIZE); 
    strncpy(input_file, argv[2], MAX_PATH_SIZE);
    data_points = parse_input_file(input_file);

    if (strcmp(goal, "sym") == 0){
        execute_goal_and_print(data_points, 1);
    }
    else if (strcmp(goal, "ddg") == 0){
        execute_goal_and_print(data_points, 2);
    }
    else{
        execute_goal_and_print(data_points, 3);
    }
    free_all_memory(symnmf_manager, 1);
    return 0;

}
