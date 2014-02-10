#!/usr/bin/env python

import frame_getter
from PIL import Image

def p(i, pts, dts, arr):
    print pts, dts
    print arr.shape
    # print arr[:10, 500:510]
    # jim = Image.fromarray(arr)
    # jim.save('ex.png')
    if i > 10:
        return pts

print 'Testing'
print dir(frame_getter)
print frame_getter.newp('../data/AML_20140114_102434.mp4', p)

# vim: et sw=4 sts=4
