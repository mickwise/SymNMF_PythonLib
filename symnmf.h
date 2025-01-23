/*
 * File: symnmf.h
 * Description: Header file for symnmf.c 
 * Author: Dor Fuchs
 */
# ifndef SYMNMF_H
# define SYMNMF_H
# include "memory_management.h"

/*Function definition*/

/*
 * Function: sym
 * --------------------------
 * Calculates the similarity matrix of the data points.
 *
 * Parameters:
 *    python_manager: A pointer to the memory manager allocated for python, if called from C a NULL value is given.
 *    data_points: A Matrix struct holding the data points, their amount(rows) and their dimension(columns)
 *    for_python: A flag indicating if the function was called through the Python-C API and needs to return the similarity matrix.
 * 
 * Memory handaling: 
 *    With the manager allocated for the Python-C API if for_python is true, 
 *    othewise with the global manager of the symnmf.c file.
 *
 * Returns:
 *    The similarity matrix.
 */
Matrix* sym(Memory_manager *python_manager, Matrix *data_points, int for_python);

/*
 * Function: ddg
 * --------------------------
 * Calculates the diagonal degree matrix of the data points.
 *
 * Parameters:
 *    python_manager: A pointer to the memory manager allocated for python, if called from C a NULL value is given.
 *    similarity_matrix: A pointer to the pre-calculated similarity matrix.
 *    data_points: A Matrix struct holding the data points, their amount(rows) and their dimension(columns)
 *    for_python: A flag indicating if the function was called through the Python-C API and needs to return the similarity matrix.
 * 
 * Memory handaling: 
 *    With the manager allocated for the Python-C API if for_python is true, 
 *    othewise with the global manager of the symnmf.c file.
 *
 * Returns:
 *    The diagonal degree matrix.
 */
double* ddg(Memory_manager *python_manager, Matrix *similarity_matrix, Matrix *data_points, int for_python);

/*
 * Function: ddg
 * --------------------------
 * Calculates the normalaized similarity matrix of the data points.
 *
 * Parameters:
 *    python_manager: A pointer to the memory manager allocated for python, if called from C a NULL value is given.
 *    similarity_matrix: A pointer to the pre-calculated similarity matrix.
 *    diagonal_degree_matrix: A pointer to the pre-calculated diagonal degree matrix.
 *    data_points: A Matrix struct holding the data points, their amount(rows) and their dimension(columns)
 *    for_python: A flag indicating if the function was called through the Python-C API and needs to return the similarity matrix.
 * 
 * Memory handaling: 
 *    With the manager allocated for the Python-C API if for_python is true, 
 *    othewise with the global manager of the symnmf.c file.
 *
 * Returns:
 *    The normalized similarity matrix.
 */
Matrix* norm(Memory_manager *python_manager, Matrix *similarity_matrix, double *diagonal_degree_matrix, Matrix *data_points, int for_python);

/*
 * Function: symnmf
 * --------------------------
 * Preforms the symNMF algorithm on a given input matrix H up to a given precision level epsilon
 *
 * Parameters:
 *    python_manager: A pointer to the memory manager allocated for python, if called from C a NULL value is given.
 *    H: The lower dimentional target matrix
 *    W: The normalized similarity matrix
 *
 * Memory handaling: 
 *    With the manager allocated for the Python-C API if for_python is true, 
 *    othewise with the global manager of the symnmf.c file.
 * 
 * Returns:
 *    The optimized target matrix H.
 */
Matrix *symnmf(Memory_manager *python_manager, Matrix *H, Matrix *W);
#endif
