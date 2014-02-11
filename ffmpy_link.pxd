
# cimport numpy as np
cimport cython

cdef extern from "./ffmpy.h":
    ctypedef struct status:
        int index
        int numerator
        int denominator
        int start_time
        int duration
        int play_time
        int decode_time
        cython.char* data
        int line_size
        int width
        int height

    ctypedef int callback(int, status)
    int getFrames(int, char* fname, callback cb)
    int other(int)

