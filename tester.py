#!/usr/bin/env python

import frame_getter

def p(time, arr, *a):
    print 'hi'
    print time
    print arr.shape
    return time > 3000

print 'Testing'
print dir(frame_getter)
print frame_getter.thing(23)
print frame_getter.newp(31, '../data/AML_20140114_102434.mp4', p)

# vim: et sw=4 sts=4
