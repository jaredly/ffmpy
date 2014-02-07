
# cimport numpy as np
cimport cython

cdef extern from "./ffmpy.h":
    ctypedef int callback(int, int, cython.char*, int, int, int)
    int open(int, char* fname, callback cb)
    int newpen(int, char* fname, callback cb)
    int other(int)

