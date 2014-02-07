#!/usr/bin/env python

import frame_getter
from PIL import Image

def p(time, arr, *a):
    print 'hi'
    print time
    print arr.shape
    print arr[:10, 500:510]
    im = Image.fromarray(arr)
    im.save('ex.png')
    return 1

print 'Testing'
print dir(frame_getter)
print frame_getter.thing(23)
print frame_getter.newp(31, '../data/AML_20140114_102434.mp4', p)

# vim: et sw=4 sts=4
