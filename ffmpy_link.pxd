
# cimport numpy as np
cimport cython

cdef extern from "./ffmpy.h":
    ctypedef int callback(int, int, int, int, int, int, int, int, cython.char*, int, int, int)
    int getFrames(int, char* fname, callback cb)
    int other(int)

