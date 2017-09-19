desc = '''
GooseEYE is a C++ module, wrapped in Python, that is aimed at computing image based geometrical
statistics in 1-, 2-, and 3-D.
'''

from setuptools import setup, Extension

import sys
import setuptools
import pybind11
import cppmat

__version__ = '0.1.2'

ext_modules = [
  Extension(
    'GooseEYE',
    ['src/core/image.cpp','src/python/image.cpp'],
    include_dirs=[
      pybind11.get_include(False),
      pybind11.get_include(True ),
      cppmat  .get_include(False),
      cppmat  .get_include(True )
    ],
    extra_compile_args = ["-DNDEBUG"], # switch off assertions
    language='c++'
  ),
]

setup(
  name               = 'GooseEYE',
  description        = 'Image based geometrical statistics',
  long_description   = desc,
  keywords           = 'Statistics, GUI, C++, C++11, Python bindings, pybind11',
  version            = __version__,
  license            = 'GPLv3',
  author             = 'Tom de Geus',
  author_email       = 'tom@geus.me',
  url                = 'https://github.com/tdegeus/GooseEYE',
  ext_modules        = ext_modules,
  install_requires   = ['pybind11>=2.1.0','cppmat>=0.2.8'],
  cmdclass           = {'build_ext': cppmat.BuildExt},
  zip_safe           = False,
)
