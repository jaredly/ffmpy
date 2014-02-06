
# cython: binding=True

cimport ffmpy_link
cimport numpy as np
cimport cython

lookup = {}
at = 0

cdef int callback(int id, np.int64_t time, np.int8_t* data, int size, int width, int height):
    print 'called', id, time
    lookup[id](time)
    # for(y=0; y<height; y++)
        # fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
    return 0

def open(item, cb):
    global at, lookup
    at += 1
    lookup[at] = cb
    print 'hello'
    return ffmpy_link.open(at, item, callback)

