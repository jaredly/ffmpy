#!/usr/bin/env python

import frame_getter
from PIL import Image

from scipy import ndimage
import numpy
import numpy as np
from pylab import *

def p(i, pts, dts, arr):
    print pts, dts
    print arr.shape
    # print arr[:10, 500:510]
    # jim = Image.fromarray(arr)
    # jim.save('ex.png')
    if i > 10:
        return pts

def findChunk(fname, limit=1000, separation=2, minsize=50 * 50):
    window = []
    bef = separation // 2
    af = int(ceil(separation / 2.0))
    def cb(i, num, den, start, dur, pts, dts, arr):
        if i > limit:
            return False
     
        window.append(arr.copy())
        if len(window) > separation + 1:
            window.pop(0)
        if len(window) >= separation:
            size, chunk = find_screens(window[:bef], window[af:])
            if size is not None:
                print i, pts, dts, size, minsize, chunk, num, den, start, dur
            if size is not None and size > minsize:
                return i, num, den, start, dur, pts, dts, size, chunk, window[-1]

    result = frame_getter.newp(fname, cb)
    if result is False:
        print 'Went %d iterations, found nothing' % limit
    return result, window

def find_screens(befores, afters, black=80, white=230):
    rgb = befores[0] < black
    for b in befores[1:]:
        rgb = numpy.logical_and(rgb, b < black)
    for a in afters:
        rgb = numpy.logical_and(rgb, a > white)
    # rgb = numpy.logical_and(before<black, after > white)
    grey = numpy.logical_and(rgb[:,:,0], rgb[:,:,1], rgb[:,:,2]).astype(int)
    objs, count = ndimage.label(grey)
    if not count:
        return None, None
    slices = ndimage.find_objects(objs)
    return biggest_slice(grey, slices)

'''
def find_screen(before, after, black=80, white=250):
    rgb = numpy.logical_and(before<black, after > white)
    grey = numpy.logical_and(rgb[:,:,0], rgb[:,:,1], rgb[:,:,2]).astype(int)
    objs, count = ndimage.label(grey)
    if not count:
        return None, None
    slices = ndimage.find_objects(objs)
    return biggest_slice(grey, slices)
'''

def biggest_slice(grey, sls):
    slices = [(grey.astype(int)[sl].sum(),sl) for sl in sls]
    slices.sort()
    return slices[-1]

print 'Testing'
print dir(frame_getter)
# print frame_getter.newp('../data/AML_20140114_102434.mp4', p)

fname = '../data/AML_20140114_102434.mp4'
if findChunk(fname, 400, minsize=500)[0]:
    print 'yes'
else:
    print 'no'

# vim: et sw=4 sts=4
