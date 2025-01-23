# SymNMF_PythonLib

## Overview
This project implements the **Symmetric Non-negative Matrix Factorization (SymNMF)** algorithm for clustering, alongside a comparative analysis with the **K-Means** algorithm. SymNMF uses matrix factorization techniques to optimize clustering solutions, while K-Means is applied as a baseline for comparison. The project is designed for efficient performance, leveraging C for computation and Python for usability and analysis.

## Features
- **SymNMF Implementation**:
  - Constructs the similarity matrix, diagonal degree matrix, and normalized similarity matrix.
  - Iteratively optimizes clustering solutions with the SymNMF algorithm.
- **K-Means Comparison**:
  - Evaluates clustering quality using silhouette scores from `sklearn.metrics`.
- **Python-C Integration**:
  - Combines Python’s flexibility with C’s computational efficiency via a Python C API extension.
- **Modular Design**:
  - Includes separate modules for computation, memory management, and analysis.

## Requirements
- Python 3.x
- GCC (for compiling the C modules)
- Python libraries:
  - `numpy`
  - `scikit-learn`

## Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/SymNMF_PythonLib.git
   cd SymNMF_PythonLib
   
2. Build the C extension:
   ```bash
   python3 setup.py build_ext --inplace
   
## Usage
1. To perform SymNMF clustering and output the HH matrix:
   ```bash
   python3 symnmf.py k symnmf input_data.txt

## Other goals
1. Compute the similarity matrix:
   ```bash
   python3 symnmf.py k sym input_data.txt

2. Compute the diagonal degree matrix:
   ```bash
   python3 symnmf.py k ddg input_data.txt

3. Compute the Normalized similarity matrix:
   ```bash
   python3 symnmf.py k norm input_data.txt

## Analyze clustering
1. Compare SymNMF with K-Means using silhouette scores:
   ```bash
   python3 analysis.py k input_data.txt
   
## File Structure

- **`symnmf.py`**: Python interface for the SymNMF algorithm.
- **`symnmf.c`**: C implementation of the algorithm.
- **`symnmfmodule.c`**: Python C API wrapper.
- **`symnmf.h`**: Header file with function prototypes.
- **`memory_management.c/h`**: Memory manager to safely allocate and free memory.
- **`symnmf_ops.c/h`**: Utility methods for matrix operations and SymNMF optimization.
- **`kmeans.py`**: Python implementation of the K-Means algorithm.
- **`analysis.py`**: Script for analyzing clustering results.
- **`setup.py`**: Build script for the Python C extension.
- **`Makefile`**: Script for compiling the C implementation.
- **`matrix.h`**: Header defining the matrix structure for mathematical operations.

## Output

The program outputs matrices in a comma-separated format:
1. Each row is on a new line.
2. Numbers are formatted to 4 decimal places.

### Example Output:
```plaintext
0.0600,0.0100
0.0100,0.0500
0.0100,0.0400
0.0200,0.0400
0.0500,0.0200

## License
This project is licensed under the MIT License.
