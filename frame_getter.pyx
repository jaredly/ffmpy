
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
    # print size, width, height
    return np.array(basic, copy=False)

cdef int callback(int id, int i, int num, int den, int start, int dur, int pts, int dts, cython.char* data, int size, int width, int height):
    # print 'called', i, pts, dts
    # for(y=0; y<height; y++)
        # fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
    arr = toArray(data, size, width, height)
    try:
        result = lookup[id](i, num, den, start, dur, pts, dts, arr)
    except Exception as e:
        print 'Callback failed', e
        lookup[id] = e
        return 1
    except KeyboardInterrupt as e:
        print 'User cancelled', e
        lookup[id] = None
        return 1
    if result is not None:
        lookup[id] = result
        return 1
    return 0

def newp(char* fname, cb):
    global at, lookup
    at += 1
    lookup[at] = cb
    res = ffmpy_link.getFrames(at, fname, callback)
    if res == -1:
        raise Exception('Failed to get frames')
    return lookup[at]

'''
datas = {}
rets = {}

def keyframe(char* fname, separation=2, minsize=50 * 50):
    global at, lookup
    at += 1
    lookup[at] = cb
    cdef cnp.ndarray[cnp.NPY_UINT8, ndim=3] window[separation]
    datas[at] = window, separation, minsize, limit

    ffmpy_link.newpen(at, fname, findKeyframe)
    return rets.get(at, None)

cdef int callback(int id, int i, int pts, int dts, cython.char* data, int size, int width, int height):
    if not id in lookup:
        return 1
    arr = toArray(data, size, width, height)

    cdef cnp.npy_intp shape[3]
    shape[0] = <cnp.npy_intp> height
    shape[1] = <cnp.npy_intp> width
    shape[2] = <cnp.npy_intp> 3
    cdef cnp.ndarray[cnp.NPY_UINT8, ndim=3] basic = cnp.PyArray_SimpleNewFromData(3, shape, cnp.NPY_UINT8, data)

    window, separation, minsize, limit = datas[id]
    if i > limit:
        return 1
    window.append(basic)
    if len(window) > separation + 1:
        window.pop(0)
    if len(window) < separation:
        return 0

    size, chunk = find_screens(window[:bef], window[af:])

    try:
        result = lookup[id](pts, dts, arr)
    except Exception as e:
        print 'Callback failed', e
        return 1
    if type(result) == int:
        return result
    return 1 if result else 0
'''

