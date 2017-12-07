desc = '''
GooseEYE is a module to perform geometrical statistics.
'''

from setuptools import setup, Extension

import sys,re
import setuptools
import pybind11
import cppmat

header = open('../core/macros.h','r').read()
world  = re.split('(.*)(\#define GOOSEEYE_WORLD_VERSION\ )([0-9]+)(.*)',header)[3]
major  = re.split('(.*)(\#define GOOSEEYE_MAJOR_VERSION\ )([0-9]+)(.*)',header)[3]
minor  = re.split('(.*)(\#define GOOSEEYE_MINOR_VERSION\ )([0-9]+)(.*)',header)[3]

__version__ = '.'.join([world,major,minor])

ext_modules = [
  Extension(
    'GooseEYE',
    ['image.cpp','../core/image.cpp'],
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
  description      = 'Geometrical statistics',
  long_description = desc,
  version          = __version__,
  license          = 'GPLv3',
  author           = 'Tom de Geus',
  author_email     = 'tom@geus.me',
  url              = 'https://github.com/tdegeus/GooseEYE',
  ext_modules      = ext_modules,
  install_requires = ['pybind11>=2.2.0','cppmat>=0.2.18'],
  cmdclass         = {'build_ext': cppmat.BuildExt},
  zip_safe         = False,
)
