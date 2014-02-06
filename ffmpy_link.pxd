
cimport numpy as np

cdef extern from "./ffmpy.h":
    ctypedef int callback(int, np.int64_t, np.int8_t*, int, int, int)
    int open(int, char* fname, callback cb)

