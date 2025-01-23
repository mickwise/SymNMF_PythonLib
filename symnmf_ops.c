/*
 * File: symnmf_utils.c
 * Description: Utility methods for performing the symNMF algroithm, mostly matrix operations.
 * Author: Dor Fuchs
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "memory_management.h"
#include "symnmf_ops.h"

/*Function declaration*/
double calculate_element(Matrix*, int, int);
double inner_product(double *, double *, int);
double calculate_numerator_element(Memory_manager*, Matrix *, Matrix *, int,int);

double calc_similarity(double *point1, double *point2, int dimension){
    /*Variable declaration*/
    int i;
    double similarity_value = 0.0;

    for (i = 0; i < dimension; i++){
        similarity_value += (point1[i] - point2[i])*(point1[i] - point2[i]);
    }

    return exp(-0.5*similarity_value);
}

void calculate_step(Memory_manager *manager, Matrix *H_t_plus_1, Matrix *H, Matrix *W){
    /*Variable declaration*/
    int i;
    int j;
    double numerator_element;
    double denominator_element;

    for (i = 0; i < H->rows; i++){
        for (j = 0; j < H->columns; j++){
            numerator_element = calculate_numerator_element(manager, H, W, i, j);
            denominator_element = calculate_element(H, i, j);
            H_t_plus_1->data[i][j] = H->data[i][j]*(0.5 + 0.5*(numerator_element/denominator_element));
        }
    }
}

double squared_frobenius_norm_of_difference(Matrix *matrix1, Matrix *matrix2){
    /*Variable declaration*/
    int i;
    int j;
    int rows = matrix1->rows;
    int columns = matrix1->columns;
    double difference;
    double frobenius_norm = 0.0;

    for (i = 0; i < rows; i++){
        for (j = 0; j < columns; j++){
            difference = matrix1->data[i][j] - matrix2->data[i][j];
             frobenius_norm += difference*difference;
        }
    }
    return frobenius_norm;
}

void transfer_data_between_matrices(Matrix *input_matrix, Matrix *target_matrix){
    /*Variable declaration*/
    int i;
    for (i = 0; i < input_matrix->rows; i++){
        memcpy(target_matrix->data[i], input_matrix->data[i], input_matrix->columns*sizeof(double));
    }
}

/*
 * Function: calculate_element
 * --------------------------
 * Calculates an element of H×Hᵀ×H.
 *
 * Parameters:
 *    H: A pointer to the matrix H
 *    row: The row number of the element calculated
 *    column: The column number of the element calculated
 *
 * Returns:
 *    The row×column element of H×Hᵀ×H.
 */
double calculate_element(Matrix *H, int row, int column){
    /*Variable declaration*/
    int i;
    int j;
    double current_column_element;
    double element = 0.0;

    for(i = 0; i < H->rows; i++){
        current_column_element = H->data[i][column];
        for(j = 0; j < H->columns; j++){
            element += H->data[row][j]*H->data[i][j]*current_column_element;
        }
    }
    return element;
}

/*
 * Function: inner_product
 * --------------------------
 * Calculates the euclidean inner product between two vectors.
 *
 * Parameters:
 *    vector1: A vector to be multiplied
 *    vector2: A vector to be multiplied
 *    dimension: The dimension of the vectors
 *
 * Returns:
 *    The euclidean inner product between vector1 and vector2.
 */
double inner_product(double *vector1, double *vector2, int dimension){
    /*Variable declaration*/
    int i;
    double inner_product = 0.0;

    for(i = 0; i < dimension; i++){
        inner_product += vector1[i]*vector2[i];
    }

    return inner_product;
}

/*
 * Function: calculate_numerator_element
 * --------------------------
 * Calculates the nomerator element for the calculate step function.
 *
 * Parameters:
 *    manager: A pointer to the given memory manager.
 *    H: The association matrix.
 *    W: The normalized similarity matrix.
 *    row: The row index of the element being calculated.
 *    column: The column index of the element being calculated.
 *
 * Returns:
 *    The ijth numerator element
 */
double calculate_numerator_element(Memory_manager *manager, Matrix *H, Matrix *W, int row ,int column){
    /*Variable declaration*/
    int i;
    double numerator_element;
    double* H_column_j;

    /*Memory allocation*/
    H_column_j = safe_malloc(manager, H->rows * sizeof(double), 1);
    for (i = 0; i < H->rows; i++) {
        H_column_j[i] = H->data[i][column];
    }
    numerator_element = inner_product(W->data[row], H_column_j, W->columns);
    return numerator_element;
}
