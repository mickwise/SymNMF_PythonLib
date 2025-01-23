/*
 * File: symnmf_utils.h
 * Description: Header file for utility methods to perform symNMF algorithm operations.
 * Author: Dor Fuchs
 */
#ifndef SYMNMF_UTILS_H
#define SYMNMF_UTILS_H
# include "memory_management.h"
# include "matrix.h"

/*Function declaration*/

/*
 * Function: calc_similarity
 * --------------------------
 * Calculates the similarity value of two distinct data points.
 *
 * Parameters:
 *    point1: A data point
 *    point2: A data point
 *    dimension: The dimension of the data points
 *
 * Returns:
 *    The similarity value of points 1 and 2.
 */
double calc_similarity(double *point1, double *point2, int dimension);

/*
 * Function: calculate_step
 * --------------------------
 * Calculates the new matrix Hₜ₊₁ by calculating each element of it sequentially 
 *
 * Parameters:
 *    H_t_plus_1: A pointer to the memory block in which Hₜ₊₁ should be saved
 *    H: A pointer to the previous iteration matrix
 *    W: A pointer to the normalized similarity matrix
 *
 * Returns:
 *    Nothing.
 */
void calculate_step(Memory_manager *manager, Matrix *H_t_plus_1, Matrix *H, Matrix *W);

/*
 * Function: squared_frobenius_norm_of_difference
 * --------------------------
 * Calculates the squared Frobenius norm of the difference between two matrices.
 *
 * Parameters:
 *    matrix1: The matrix to be subtracted from
 *    matrix2: The subtracted matrix
 *
 * Returns:
 *    The squared Frobenius norm matrix1 - matrix2.
 */
double squared_frobenius_norm_of_difference(Matrix *matrix1, Matrix *matrix2);

/*
 * Function: transfer_data_between_matrices
 * --------------------------
 * Transfers the data from the input_matrix to the target_matrix row by row.
 *
 * Parameters:
 *    input_matrix: A pointer to the matrix data is transferred from
 *    target_matrix: A pointer to the matrix data is transferred to
 *
 * Returns:
 *    Nothing.
 */
void transfer_data_between_matrices(Matrix *input_matrix, Matrix *target_matrix);
#endif
