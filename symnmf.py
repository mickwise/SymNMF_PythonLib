"""
Implementation of the symNMF algorithm with the C-Python API
"""
import sys
import math
import numpy as np
import symnmf_extension

np.random.seed(1234)


def extract_cmd():
    """
    Extracts command line arguments from the user.

    Parameters:
        None.

    Returns:
        A tuple containing the number of clusters, goal and input file path.
    """
    if len(sys.argv) != 4:
        exit_error()
    return int(sys.argv[1]), sys.argv[2], sys.argv[3]

def parse_input_file(input_file):
    """
    Parses the data points from the input file.

    Parameters:
        input_file: The path to the imput file holding the data points.

    Returns:
        A 2-dimensional list containing the data points.
    """
    with open(input_file, "r") as open_file:
        data_points = [[float(value) for value in line.strip().split(",")] for line in open_file]
    return data_points

def initialize_association_matrix(normalized_similarity_matrix, number_of_clusters):
    """
    Initializes the association matrix for SymNMF.
    
    Parameters:
        normalized_similarity_matrix: 2-dimensional python list containing 
        the normalized similarity matrix.
        number_of_clusters: Number of clusters to initialize the matrix for.

    Returns:
        association_matrix: A 2-dimensional list with values randomly 
        initialized between [0, 2 * sqrt(m/k)] where m is the 
        element-wise average for the normalized similarity matrix 
        and k is the number of clusters.
    """
    normalized_similarity_matrix_average = calculate_matrix_average(normalized_similarity_matrix)
    upper_bound = 2*math.sqrt(normalized_similarity_matrix_average/number_of_clusters)
    rows = len(normalized_similarity_matrix)
    columns = number_of_clusters
    association_matrix = [[np.random.uniform(0, upper_bound) for _ in range(columns)] for _ in range(rows)]
    return association_matrix

def calculate_matrix_average(matrix):
    """
    Calculates the element-wise average of the given matrix.

    Parameters:
        matrix: A 2-dimensional list containing the matrix' values.

    Returns:
        The given matrix element-wise average.
    """
    sum_of_elements = sum(sum(row) for row in matrix)
    number_of_elements = len(matrix)*len(matrix[0])
    return sum_of_elements/number_of_elements

def print_output_matrix(matrix, is_diagonal):
    """
    Print the output matrix in a CSV format.

    Parameters:
        matrix: A 2-dimensional list containing the matrix' values.
        is_diagonal: A flag indicating if the given matrix is diagonal.

    Returns:
        Nothing.
    """
    if is_diagonal:
        for i, value in enumerate(matrix):
            print(",".join(f"{(value if i == j else 0):.4f}" for j in range(len(matrix))))
    else:
        for row in matrix:
            print(",".join(f"{value:.4f}" for value in row))

def exit_error():
    """
    Send out error message, as specified on the assignments instructions page 
    and exit unsuccessfully.

    Parameters:
        None.

    Returns:
        Nothing.
    """
    print("An Error Has Occurred")
    sys.exit(1)

def main():
    """
    Run one of four possible tasks:
    1. sym: Calculate and print the similarity matrix.
    2. ddg: Calculate and print the Diagonal Degree Matrix.
    3. norm: Calculate and print the normalized similarity matrix.
    4. symnmf: Perform the full symNMF algorithm and print the finale association matrix.

    Parameters:
        None.

    Returns:
        Nothing.
    """
    number_of_clusters, goal, input_file = extract_cmd()
    data_points = parse_input_file(input_file)

    if goal == "sym":
        similarity_matrix = symnmf_extension.sym(data_points)
        print_output_matrix(similarity_matrix, False)

    elif goal == "ddg":
        diagonal_degree_matrix = symnmf_extension.ddg(data_points)
        print_output_matrix(diagonal_degree_matrix, True)

    else:
        normalized_similarity_matrix = symnmf_extension.norm(data_points)
        if goal == "norm":
            print_output_matrix(normalized_similarity_matrix, False)
        else:
            association_matrix = initialize_association_matrix(normalized_similarity_matrix, number_of_clusters)
            output_matrix = symnmf_extension.symnmf(association_matrix, normalized_similarity_matrix)
            print_output_matrix(output_matrix, False)

if __name__ == "__main__":
    main()
    