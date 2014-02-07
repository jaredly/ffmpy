#!/usr/bin/env python
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import numpy

import subprocess

LINK = subprocess.check_output('pkg-config --libs libavformat libavcodec libswscale libavutil sdl'.split()) + ' -lm'
COMP = subprocess.check_output('pkg-config --cflags libavformat libavcodec libswscale libavutil sdl'.split())

setup(
    name = 'ffmpy',
    cmdclass = {'build_ext': build_ext},
    ext_modules = [
        Extension(
            name="frame_getter",
            sources=["frame_getter.pyx", 'ffmpy.c'],
            include_dirs=[numpy.get_include()],
            language='c',
            extra_compile_args = COMP.split(),
            extra_link_args = LINK.split()

            # libraries=['ffmpy']
        )
    ]
)

# vim: et sw=4 sts=4
