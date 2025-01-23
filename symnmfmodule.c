# define PY_SSIZE_T_CLEAN
# include <Python.h>
# include <string.h>
# include "symnmf_ops.h"
# include "symnmf.h"
# include "memory_management.h"

/*Function declaration*/
static PyObject* py_sym(PyObject*, PyObject*);
static PyObject* py_ddg(PyObject*, PyObject*);
static PyObject* py_norm(PyObject*, PyObject*);
static PyObject* py_symnmf(PyObject*, PyObject*);
void parse_objects_from_python(PyObject*, PyObject**, PyObject**, int);
double** parse_float_list(PyObject*, int, int);
void transfer_data_from_list(double**, PyObject*, int, int);
int* extract_list_dimensions(PyObject*);
PyObject* transform_1d_array_to_python_list(double *, int);
PyObject* transform_2d_array_to_python_list(double **, int, int);
Matrix* initialize_data_points_and_manager(PyObject*);

/*Globlal variables declaration*/
Memory_manager *module_manager;

static PyMethodDef method_table[] = {
    {
        "sym",
        py_sym,
        METH_VARARGS,
        "Calculates the similarity matrix of the data points.\n\n"
        "Parameters:\n"
        "data_points: A 2-dimensional Python list of real data points.\n\n"
        "Returns:\n"
        "The similarity matrix as a 2-dimensional Python list."
    }, {
        "ddg",
        py_ddg,
        METH_VARARGS,
        "Calculates the diagonal degree matrix of the data points.\n\n"
        "Parameters:\n"
        "data_points: A 2-dimensional Python list of real data points.\n\n"
        "Returns:\n"
        "The diagonal degree matrix as a 1-dimensional Python list."
    }, {
        "norm",
        py_norm,
        METH_VARARGS,
        "Calculates the normalized similarity matrix of the data points.\n\n"
        "Parameters:\n"
        "data_points: A 2-dimensional Python list of real data points.\n\n"
        "Returns:\n"
        "The normalized similarity matrix as a 2-dimensional Python list."
    }, {
        "symnmf",
        py_symnmf,
        METH_VARARGS,
        "Performs the symNMF algorithm on a given pre-calculated matrix H and returns the calculated matrix.\n\n"
        "Parameters:\n"
        "H: A 2-dimensional Python list holding the pre-calculated correlation values between data points.\n"
        "W: A 2-dimensional Python list holding the normalized similarity matrix of the data points.\n\n"
        "Returns:\n"
        "The optimized correlation matrix as a 2-dimensional Python list."
    }, {
        NULL, NULL, 0, NULL
    }
};

static struct PyModuleDef symnmfmodule = {
    PyModuleDef_HEAD_INIT,
    "symnmf_extension",
     "This module provides implementations for the symNMF algorithm and related matrix operations.\n\n"
    "Functions:\n"
    "  sym(data_points):\n"
    "    Calculates the similarity matrix of the data points.\n\n"
    "  ddg(data_points):\n"
    "    Computes the diagonal degree matrix of the data points.\n\n"
    "  norm(data_points):\n"
    "    Calculates the normalized similarity matrix of the data points.\n\n"
    "  symnmf(H, W):\n"
    "    Performs the symNMF algorithm on a given pre-calculated matrix H and returns the calculated matrix.",
    -1,
    method_table

};

PyMODINIT_FUNC PyInit_symnmf_extension(void){
    PyObject *m;
    m = PyModule_Create(&symnmfmodule);
    if (!m) {
        return NULL;
    }
    return m;
}

/*
 * Function: sym
 * --------------------------
 * Calculates the similarity matrix of the data points.
 *
 * Parameters:
 *    data_points: A 2-dimensional Python list of real data points.
 * 
 * Memory Handling: 
 *    Uses a dedicated memory manager that frees all non-Python allocated memory before returning.
 *
 * Returns:
 *    The similarity matrix as a 2-dimensional Python list.
 */
static PyObject* py_sym(PyObject* self, PyObject* args){
    /*Variable declaration*/
    Matrix *data_points;
    Matrix *output_matrix;
    PyObject *output_list;

    data_points = initialize_data_points_and_manager(args);
    output_matrix = sym(module_manager, data_points, 1);
    output_list = transform_2d_array_to_python_list(output_matrix->data, output_matrix->rows, output_matrix->columns);
    free_all_memory(module_manager, 1);
    return output_list;
}

/*
 * Function: ddg
 * --------------------------
 * Calculates the diagonal degree matrix of the data points.
 *
 * Parameters:
 *    data_points: A 2-dimensional Python list of real data points.
 * 
 * Memory Handling: 
 *    Uses a dedicated memory manager that frees all non-Python allocated memory before returning.
 *
 * Returns:
 *    The diagonal degree matrix as a 1-dimensional Python list.
 */
static PyObject* py_ddg(PyObject* self, PyObject* args){
    /*Variable declaration*/
    Matrix *data_points;
    Matrix *similarity_matrix;
    double *output_matrix;
    PyObject *output_list;

    data_points = initialize_data_points_and_manager(args);
    similarity_matrix = sym(module_manager, data_points, 1);
    output_matrix = ddg(module_manager, similarity_matrix, data_points, 1);
    output_list = transform_1d_array_to_python_list(output_matrix, similarity_matrix->rows);
    free_all_memory(module_manager, 1);
    return output_list;
}

/*
 * Function: norm
 * --------------------------
 * Calculates the normalized similarity matrix of the data points.
 *
 * Parameters:
 *    data_points: A 2-dimensional Python list of real data points.
 * 
 * Memory Handling: 
 *    Uses a dedicated memory manager that frees all non-Python allocated memory before returning.
 *
 * Returns:
 *    The normalized similarity matrix as a 2-dimensional Python list.
 */
static PyObject* py_norm(PyObject* self, PyObject* args){
    /*Variable declaration*/
    Matrix *data_points;
    Matrix *similarity_matrix;
    double *diagonal_degree_matrix;
    Matrix *output_matrix;
    PyObject *output_list;

    data_points = initialize_data_points_and_manager(args);
    similarity_matrix = sym(module_manager, data_points, 1);
    diagonal_degree_matrix = ddg(module_manager, similarity_matrix, data_points, 1);
    output_matrix = norm(module_manager, similarity_matrix, diagonal_degree_matrix, data_points, 1);
    output_list = transform_2d_array_to_python_list(output_matrix->data, similarity_matrix->rows, similarity_matrix->columns);
    free_all_memory(module_manager, 1);
    return output_list;
}

/*
 * Function: symnmf
 * --------------------------
 * Performs the symNMF algorithm on a given pre-calculated matrix H and returns the calculated matrix.
 *
 * Parameters:
 *    H: A 2-dimensional Python list holding the pre-calculated correlation values between data points.
 *    W: A 2-dimensional Python list holding the normalized similarity matrix of the data points.
 * Memory Handling: 
 *    Uses a dedicated memory manager that frees all non-Python allocated memory before returning.
 *
 * Returns:
 *    The optimized correlation matrix as a 2-dimensional Python list.
 */
static PyObject* py_symnmf(PyObject *self, PyObject *args){
    /*Variable declaration*/
    int *dimensions;
    Matrix *H;
    Matrix *W;
    Matrix *output_matrix;
    PyObject *raw_H_data;
    PyObject *raw_W_data;
    PyObject *output_list;

    /*Memory allocation*/
    initialize_manager(&module_manager);
    parse_objects_from_python(args, &raw_H_data, &raw_W_data, 1);
    dimensions = extract_list_dimensions(raw_H_data);
    H = safe_initialize_matrix(module_manager, dimensions[0], dimensions[1]);
    W = safe_initialize_matrix(module_manager, dimensions[0], dimensions[0]);

    H->data = parse_float_list(raw_H_data, dimensions[0], dimensions[1]);
    W->data = parse_float_list(raw_W_data, dimensions[0], dimensions[0]);
    output_matrix = symnmf(module_manager, H, W);
    output_list = transform_2d_array_to_python_list(output_matrix->data, output_matrix->rows, output_matrix->columns);
    free_all_memory(module_manager, 1);
    return output_list;
}

/*
 * Function: parse_objects_from_python
 * --------------------------
 * Parses arguments received from Python and saves them into provided C variables. 
 * 
 * Parameters:
 *   args: The arguments passed from Python.
 *   pointer_1: A pointer to the PyObject* into which to save a Python list.
 *   pointer_2: A pointer to the PyObject* into which to save a Python list.
 *   is_symnmf: A flag that indicates whether to parse one or two arguments from Python.
 *              0 - If called from sym, ddg or norm.
 *              1 - If called from symnmf.
 *
 * Returns:
 *    Nothing.
 */
void parse_objects_from_python(PyObject *args, PyObject **pointer_1, PyObject **pointer_2, int is_symnmf){
    if (is_symnmf){
        if(PyArg_ParseTuple(args, "OO", pointer_1, pointer_2) != 1){
        printf("An Error Has Occurred");
        free_all_memory(module_manager, 1);
        exit(EXIT_FAILURE);
        }
    }
    else{
        if(PyArg_ParseTuple(args, "O", pointer_1) != 1){
        printf("An Error Has Occurred");
        free_all_memory(module_manager, 1);
        exit(EXIT_FAILURE);
        }
    }
}

/*
 * Function: parse_float_list
 * --------------------------
 * Allocate memory for and transfer data to a 2-dimensional double array from a Python float list.
 *
 * Parameters:
 *    raw_list: A pointer to the Python list saved as a PyObject.
 *
 * Returns:
 *    The 2-dimensional double array holding the data parsed from the Python list.
 */
double** parse_float_list(PyObject *raw_list, int number_of_rows, int number_of_columns){
    /*Variable declaration*/
    int i;
    double **list_data;

    /*Memory allocation*/
    list_data = safe_malloc(module_manager, number_of_rows*sizeof(double*), TWO_DIMENSIONS);
    for (i = 0; i < number_of_rows; i++){
        list_data[i] = safe_malloc(module_manager, number_of_columns*sizeof(double), ONE_DIMENSION);
    }

    transfer_data_from_list(list_data, raw_list, number_of_rows, number_of_columns);
    return list_data;

}

/*
 * Function: transfer_data_from_list
 * --------------------------
 * Parses a Python 2-dimensional float list given as a PYObject into a 2-dimensional double array.
 *
 * Parameters:
 *    data: The 2-dimensional double array in which the lists data should be saved.
 *    raw_data: The given Python list.
 *    number_of_rows: The amount of rows in the raw_data list.
 *    number_of_columns: The amount of columns in the raw_data list.
 *
 * Returns:
 *    Nothing.
 */
void transfer_data_from_list(double **data, PyObject *raw_data, int number_of_rows, int number_of_columns){
    /*Variable declaration*/
    int i;
    int j;
    PyObject *current_sub_array;

    for (i = 0; i < number_of_rows; i++){
        current_sub_array = PyList_GetItem(raw_data, i);
        for (j = 0; j < number_of_columns; j++){
            data[i][j] = PyFloat_AS_DOUBLE(PyList_GetItem(current_sub_array, j));
        }
    }
}

/*
 * Function: extract_list_dimensions
 * --------------------------
 * Extracts the amount of rows and columns of a 2-dimensional Python list.
 *
 * Parameters:
 *   raw_list: The 2-dimensional Python list saved as a PyObject.
 *
 * Returns:
 *    A size 2 int array holding the number of rows in the first slot and the number of columns in the second.
 */
int* extract_list_dimensions(PyObject *raw_list){
    /*Variable declaration*/
    int *dimensions;

    /*Memory allocation*/
    dimensions = safe_malloc(module_manager, 2*sizeof(int), ONE_DIMENSION);

    dimensions[0] = (int)PyList_Size(raw_list);
    dimensions[1] = (int)PyList_Size(PyList_GetItem(raw_list, 0));

    return dimensions;
}

/*
 * Function: transform_1d_array_to_python_list
 * --------------------------
 * Transforms a 1-dimensional array of doubles into a PyObject and returns it.
 *
 * Parameters:
 *   input_array: The 1-dimensional array of doubles whose data is to be saved in a PyObject.
 *   number_of_elements: The number of elements in the input array.
 *
 * Returns:
 *    A PyObject holding the data of the input array as a 1-dimensional list.
 */
PyObject* transform_1d_array_to_python_list(double *input_array, int number_of_elements){
    /*Variable declaration*/
    int i ;
    PyObject *current_element;
    PyObject *output_array;

    output_array = PyList_New(number_of_elements);
    allocation_error_check_and_exit(module_manager ,output_array);
    for (i = 0; i < number_of_elements; i++){
        current_element = PyFloat_FromDouble(input_array[i]);
        allocation_error_check_and_exit(module_manager, current_element);
        PyList_SetItem(output_array, i, current_element);
    }
    return output_array;
}

/*
 * Function: transform_2d_array_to_python_list
 * --------------------------
 * Transforms a 2-dimensional array of doubles into a PyObject and returns it.
 *
 * Parameters:
 *   input_array: The 2-dimensional array of doubles whose data is to be saved in a PyObject.
 *   number_of_rows: The number of rows of the input array.
 *   number_of_columns: The number of columns of the input array.
 *
 * Returns:
 *    A PyObject holding the data of the input array as a 2-dimensional list.
 */
PyObject* transform_2d_array_to_python_list(double **input_array, int number_of_rows, int number_of_columns){
    /*Variable declaration*/
    int i;
    int j;
    PyObject *output_list;
    PyObject *current_row;
    PyObject *current_element;

    output_list = PyList_New(number_of_rows);
    allocation_error_check_and_exit(module_manager, output_list);
    for (i = 0; i < number_of_rows; i++){
        current_row = PyList_New(number_of_columns);
        allocation_error_check_and_exit(module_manager ,current_row);
        for (j = 0; j < number_of_columns; j++){
            current_element = PyFloat_FromDouble(input_array[i][j]);
            allocation_error_check_and_exit(module_manager, current_element);
            PyList_SetItem(current_row, j, current_element);
        }
        PyList_SetItem(output_list, i, current_row);
    }
    return output_list;  
}

/*
 * Function: initialize_data_points_and_manager
 * --------------------------
 * Transfers data from the given data points list into a matrix struct and initializes the memory manager.
 *
 * Parameters:
 *   args: The input parameters passed from Python.
 *
 * Returns:
 *    A Matrix struct holding the given data points.
 */
Matrix* initialize_data_points_and_manager(PyObject* args){
    /*Variable declaration*/
    int *dimensions;
    Matrix *data_points;
    PyObject *raw_data_points;

    /*Memory allocation*/
    initialize_manager(&module_manager);
    parse_objects_from_python(args, &raw_data_points, NULL, 0);
    dimensions = extract_list_dimensions(raw_data_points);
    data_points = safe_initialize_matrix(module_manager, dimensions[0], dimensions[1]);

    data_points->data = parse_float_list(raw_data_points, dimensions[0], dimensions[1]);
    return data_points;
}
