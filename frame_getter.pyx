
# cython: binding=True

cimport ffmpy_link
# cimport numpy as np
cimport cython

lookup = {}
at = 0

cdef int callback(int id, int time, cython.char* data, int size, int width, int height):
    print 'called', id, time
    try:
        # for(y=0; y<height; y++)
            # fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
        result = lookup[id](time)
    except Exception as e:
        print 'Callback failed', e
        return 1
    if type(result) == int:
        return result
    return 1 if result else 0

def newp(num, char* fname, cb):
    global at, lookup
    at += 1
    lookup[at] = cb
    return ffmpy_link.newpen(at, fname, callback)

def thing(num):
    return ffmpy_link.other(num)

