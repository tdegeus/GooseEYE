desc = '''
GooseEYE is a C++ module, wrapped in Python, that is aimed at computing image based geometrical
statistics in 1-, 2-, and 3-D.
'''

from setuptools import setup, Extension

import re
import os
import pybind11
import pyxtensor

header = open('include/GooseEYE/config.h','r').read()
major = re.split(r'(.*)(\#define GOOSEEYE_VERSION_MAJOR\ )([0-9]+)(.*)', header)[3]
minor = re.split(r'(.*)(\#define GOOSEEYE_VERSION_MINOR\ )([0-9]+)(.*)', header)[3]
patch = re.split(r'(.*)(\#define GOOSEEYE_VERSION_PATCH\ )([0-9]+)(.*)', header)[3]

__version__ = '.'.join([major, minor, patch])

ext_modules = [
  Extension(
    'GooseEYE',
    ['python/main.cpp'],
    include_dirs=[
      os.path.abspath('include/'),
      pyxtensor.find_pyxtensor(),
      pyxtensor.find_pybind11(),
      pyxtensor.find_xtensor(),
      pyxtensor.find_xtl()],
    language='c++'
  ),
]

setup(
  name = 'GooseEYE',
  description = 'Image based geometrical statistics',
  long_description = desc,
  keywords = 'Statistics, GUI, C++, C++11, Python bindings, pybind11',
  version = __version__,
  license = 'GPLv3',
  author = 'Tom de Geus',
  author_email = 'tom@geus.me',
  url = 'https://github.com/tdegeus/GooseEYE',
  ext_modules = ext_modules,
  install_requires = ['pybind11>=2.2.0', 'pyxtensor>=0.1.1'],
  cmdclass = {'build_ext': pyxtensor.BuildExt},
  zip_safe = False,
)
