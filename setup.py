"""
This module provides a Python interface for the symNMF algorithm and related matrix operations.

The functions exposed by this module are:
  - sym(data_points): Calculates the similarity matrix for given data points.
  - ddg(data_points): Computes the diagonal degree matrix for given data points.
  - norm(data_points): Calculates the normalized similarity matrix for given data points.
  - symnmf(H, W): Performs the symNMF algorithm using given pre-calculated matrices H and W.
"""
from setuptools import Extension, setup

module = Extension(
    "symnmf_extension",  
    sources=[
        'symnmfmodule.c',      # Python C extension interface
        'symnmf.c',            # sym, ddg, norm and the full symNMF implementation
        'memory_management.c', # Memory management functions and the memory manager struct
        'symnmf_ops.c'         # Mathematical functions and the matrix struct
    ]
)

setup(
    name='symnmf_extension',
    version='1.0',
    description='Python wrapper for custom C extension implementing symNMF and related operations',
    ext_modules=[module]
)
