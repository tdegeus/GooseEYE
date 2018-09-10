desc = '''
GooseEYE is a C++ module, wrapped in Python, that is aimed at computing image based geometrical
statistics in 1-, 2-, and 3-D.
'''

from setuptools import setup, Extension

import sys, re
import setuptools
import pybind11
import cppmat

header = open('include/GooseEYE/include.h','r').read()
world  = re.split(r'(.*)(\#define GOOSEEYE_WORLD_VERSION\ )([0-9]+)(.*)',header)[3]
major  = re.split(r'(.*)(\#define GOOSEEYE_MAJOR_VERSION\ )([0-9]+)(.*)',header)[3]
minor  = re.split(r'(.*)(\#define GOOSEEYE_MINOR_VERSION\ )([0-9]+)(.*)',header)[3]

__version__ = '.'.join([world,major,minor])

ext_modules = [
  Extension(
    'GooseEYE',
    ['include/GooseEYE/python.cpp'],
    include_dirs=[
      pybind11.get_include(False),
      pybind11.get_include(True ),
      cppmat  .get_include(False),
      cppmat  .get_include(True ),
    ],
    language='c++'
  ),
]

setup(
  name             = 'GooseEYE',
  description      = 'Image based geometrical statistics',
  long_description = desc,
  keywords         = 'Statistics, GUI, C++, C++11, Python bindings, pybind11',
  version          = __version__,
  license          = 'GPLv3',
  author           = 'Tom de Geus',
  author_email     = 'tom@geus.me',
  url              = 'https://github.com/tdegeus/GooseEYE',
  ext_modules      = ext_modules,
  install_requires = ['pybind11>=2.2.0','cppmat>=1.0.6'],
  cmdclass         = {'build_ext': cppmat.BuildExt},
  zip_safe         = False,
)
