# file: frame_getter.pxd

cimport numpy as np

cdef extern from "./ffmpy.h":
    ctypedef int callback(np.int64_t, np.int8_t*, int, int, int, int)
    int open(char* fname, callback cb)


