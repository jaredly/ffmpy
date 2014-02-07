#!/usr/bin/env python

import frame_getter

def p(time, *a):
    print 'hi'
    print time

print 'Testing'
print frame_getter.open('../data/AML_20140114_102434.mp4', p)
print dir(frame_getter)
print frame_getter.thing(23)

# vim: et sw=4 sts=4
