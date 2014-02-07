
cimport ffmpy_link
cimport numpy as cnp
cimport cython
import numpy as np

cnp.import_array()

lookup = {}
at = 0

cdef object toArray(cython.char* data, int size, int width, int height):
    cdef cnp.npy_intp shape[3]
    shape[0] = <cnp.npy_intp> height
    shape[1] = <cnp.npy_intp> width
    shape[2] = <cnp.npy_intp> 3
    basic = cnp.PyArray_SimpleNewFromData(3, shape, cnp.NPY_UINT8, data)
    print size, width, height

    '''
    arr = np.zeros((height, width, 3), dtype=np.uint8)
    cdef int x, y
    for y in range(height):
        for x in range(width):
            arr[y][x][0] = (<cnp.uint8_t*>(data + y*size))[0]
            arr[y][x][1] = (<cnp.uint8_t*>(data + y*size + 1))[0]
            arr[y][x][2] = (<cnp.uint8_t*>(data + y*size + 2))[0]
            '''
    return np.array(basic, copy=False)

cdef object toArray1(cython.char* data, int size, int width, int height):
    arr = np.zeros((height, width, 3), dtype=np.uint8)
    cdef int x, y
    for y in range(height):
        for x in range(width):
            arr[y][x][0] = (<cnp.uint8_t*>(data + y*size))[0]
            arr[y][x][1] = (<cnp.uint8_t*>(data + y*size + 1))[0]
            arr[y][x][2] = (<cnp.uint8_t*>(data + y*size + 2))[0]

cdef int callback(int id, int time, cython.char* data, int size, int width, int height):
    print 'called', id, time
    # for(y=0; y<height; y++)
        # fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
    arr = toArray(data, size, width, height)
    try:
        result = lookup[id](time, arr)
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

