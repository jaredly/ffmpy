
# cimport numpy as np

cdef extern from "./ffmpy.h":
    ctypedef int callback(int, int, int*, int, int, int)
    int open(int, char* fname, callback cb)
    int other(int)

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
