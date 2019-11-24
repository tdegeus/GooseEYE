Compiling
=========

Introduction
------------

This module is header only. So one just has to ``#include <GooseEYE/GooseEYE.h>``. somewhere in the source code, and to tell the compiler where the header-files are. For the latter, several ways are described below. Note that GooseEYE uses `xtensor <https://github.com/QuantStack/xtensor>`_ in virtually all functions, so you have to tell your compiler also where to find `xtensor <https://github.com/QuantStack/xtensor>`_.

.. tip::

  Optimisation of crucial importance if you do not want to wait forever. Please `use the strategies provided by xtensor <https://xtensor.readthedocs.io/en/latest/build-options.html>`_. In particular, it is highly advice to use `xsimd <https://github.com/QuantStack/xsimd>`_ in addition to the usual optimisation flags.

Manual compiler flags
---------------------

GNU / Clang
^^^^^^^^^^^

Add the following compiler's arguments (in addition to the arguments to include `xtensor <https://github.com/QuantStack/xtensor>`_):

.. code-block:: bash

  -I${PATH_TO_GOOSEEYE}/include

.. note:: **(Not recommended)**

  If you want to avoid separately including the header files using a compiler flag, ``git submodule`` is a nice way to go:

  1.  Include this module as a submodule using ``git submodule add https://github.com/tdegeus/GooseEYE.git``.

  2.  Replace the first line of this example by ``#include "GooseEYE/include/GooseEYE/GooseEYE.h"``.

      *If you decide to manually copy the header file, you might need to modify this relative path to your liking.*

  Or see :ref:`compile_automatic`. You can also combine the ``git submodule`` with any of the below compiling strategies.

.. _compile_automatic:

(Semi-)Automatic compiler flags
-------------------------------

Install
^^^^^^^

To enable (semi-)automatic build, one should 'install' ``GooseEYE`` somewhere.

Install system-wide (root)
::::::::::::::::::::::::::

1.  Proceed to a (temporary) build directory. For example

    .. code-block:: bash

      $ cd /path/to/GooseEYE/include/build

2.  'Build' ``GooseEYE``

    .. code-block:: bash

      $ cmake ..
      $ make install

    (If you've used another build directory, change the first command to ``$ cmake /path/to/GooseEYE``)

Install in custom location (user)
:::::::::::::::::::::::::::::::::

1.  Proceed to a (temporary) build directory. For example

    .. code-block:: bash

      $ cd /path/to/GooseEYE/include/build

2.  'Build' ``GooseEYE`` to install it in a custom location

    .. code-block:: bash

      $ mkdir /custom/install/path
      $ cmake .. -DCMAKE_INSTALL_PREFIX:PATH=/custom/install/path
      $ make install

    (If you've used another build directory, change the first command to ``$ cmake /path/to/GooseEYE``)

3.  Add the following path to your ``~/.bashrc`` (or ``~/.zshrc``):

    .. code-block:: bash

      export PKG_CONFIG_PATH=/custom/install/path/share/pkgconfig:$PKG_CONFIG_PATH

.. note:: **(Not recommended)**

  If you do not wish to use ``CMake`` for the installation, or you want to do something custom. You can of course. Follow these steps:

  1.  Copy the file ``src/GooseEYE.pc.in`` to ``GooseEYE.pc`` to some location that can be found by ``pkg_config`` (for example by adding ``export PKG_CONFIG_PATH=/path/to/GooseEYE.pc:$PKG_CONFIG_PATH`` to the ``.bashrc``).

  2.  Modify the line ``prefix=@CMAKE_INSTALL_PREFIX@`` to ``prefix=/path/to/GooseEYE``.

  3.  Modify the line ``Cflags: -I${prefix}/@INCLUDE_INSTALL_DIR@`` to ``Cflags: -I${prefix}/include``.

  4.  Modify the line ``Version: @GOOSEEYE_VERSION_NUMBER@`` to reflect the correct release version.

Compiler arguments from 'pkg-config'
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Instead of ``-I...`` one can now use

.. code-block:: bash

  `pkg-config --cflags GooseEYE` -std=c++14

as compiler argument.

Compiler arguments from 'cmake'
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Add the following to your ``CMakeLists.txt``:

.. code-block:: cmake

  set(CMAKE_CXX_STANDARD 14)

  find_package(xtl REQUIRED)
  find_package(xtensor REQUIRED)
  find_package(GooseEYE REQUIRED)

  add_executable(main ...)
  target_link_libraries(main xtensor xtl GooseEYE)

Note that the following can greatly increase speed:

.. code-block:: cmake

  find_package(xsimd REQUIRED)

  ...

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DXTENSOR_USE_XSIMD=ON")

  ...

  target_link_libraries(main ... xsimd)


