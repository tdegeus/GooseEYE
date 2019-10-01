
.. _cpp:

*************
C++ interface
*************

Introduction
============

The C++ code is best explored by simply inspecting [:download:`GooseEYE.h <../include/GooseEYE/GooseEYE.h>`] here you can find some comments to get you going. To include use

.. code-block:: cpp

    #include <GooseEYE/GooseEYE.h>

Everything is contained in the namespace ``GooseEYE``.

Ensemble or individual image
============================

There are two modes of using the code. One can compute the statistics based on:

*   An individual image.
*   An ensemble of images.

The actual computation is contained in the computation of the ensemble, around which a wrapper is provided to evaluate an individual image. The computation of the ensemble proceeds in three steps:

1.   Initialize the ensemble, defining some settings of which the shape of the region-of-interest is mandatory. For example:

     .. code-block:: cpp

        GooseEYE::Ensemble ensemble({51,51});

2.  Add the statistics by evaluating the different images in the ensemble. For example:

    .. code-block:: cpp

        ensemble.S2(...);
        ...

3.  Evaluate the result:

    .. code-block:: cpp

        cppmat::array<double> result = ensemble.result();

    .. note::

        The variance around the average can be obtained using ``ensemble.variance()``

    .. note::

        To obtain the raw result and normalisation use:

        .. code-block:: cpp

            ensemble.data_first();  // first moment: x_1 + x_2 + ...
            ensemble.data_second(); // second moment: x_1^2 + x_2^2 + ...
            ensemble.norm();        // normalisation (number of measurements)

Using the individual images wrapper all these steps are combined in a single function call with almost the same arguments as the underlying ``GooseEYE::Ensemle`` functions. The only limitation is the the raw data and normalization cannot be accessed.

Statistics
==========

.. note::

  The functions are available directly in the ``GooseEYE`` namespace for individual images, and as member functions of the ``Ensemble``-class.

mean
----

The arithmetic mean. An overload is available to mask certain voxels.

S2
--

:ref:`theory_S2`. Overloads are available for ``cppmat::array<int>`` (binary and integer) images and ``cppmat::array<double>`` images, and for masked images.

W2
--

:ref:`theory_W2`. Overloads are available for different combinations of ``cppmat::array<int>`` (binary and integer) images and ``cppmat::array<double>`` images, and for masked images.

W2c
---

Collapsed weighted correlation (see: :ref:`theory_W2`). Overloads are available for ``cppmat::array<int>`` (binary and integer) images and ``cppmat::array<double>`` images, and for masked images. To automatically compute the clusters and their centres use ``W2c_auto``.

L
-

:ref:`theory_L`.

Miscellaneous functions
=======================

clusters
--------

Identify the clusters in a binary images.

clusterCenters
--------------

Identify the clusters and their centres in a binary images.

dilate
------

Dilate a binary or integer image.

kernel
------

Define a kernel.

path
----

Define a path between two voxels.

stampPoints
-----------

Return the voxel-paths use in the computation of the lineal path function and collapsed weighted correlation.

dummy_circles
-------------

Create a dummy binary images of circles.

Compiling
=========

Introduction
------------

This module is header only. So one just has to ``#include <GooseEYE/GooseEYE.h>``. somewhere in the source code, and to tell the compiler where the header-files are. For the latter, several ways are described below.

Before proceeding, a words about optimization. Of course one should use optimization when compiling the release of the code (``-O2`` or ``-O3``). But it is also a good idea to switch off the assertions in the code (mostly checks on size) that facilitate easy debugging, but do cost time. Therefore, include the flag ``-DNDEBUG``. Note that this is all C++ standard. I.e. it should be no surprise, and it always a good idea to do.

Manual compiler flags
---------------------

GNU / Clang
^^^^^^^^^^^

Add the following compiler's arguments:

.. code-block:: bash

  -I${PATH_TO_GOOSEEYE}/src -std=c++14

.. note:: **(Not recommended)**

  If you want to avoid separately including the header files using a compiler flag, ``git submodule`` is a nice way to go:

  1.  Include this module as a submodule using ``git submodule add https://github.com/tdegeus/GooseEYE.git``.

  2.  Replace the first line of this example by ``#include "GooseEYE/src/GooseEYE/GooseEYE.h"``.

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

      $ cd /path/to/GooseEYE/src/build

2.  'Build' ``GooseEYE``

    .. code-block:: bash

      $ cmake ..
      $ make install

    (If you've used another build directory, change the first command to ``$ cmake /path/to/GooseEYE/src``)

Install in custom location (user)
:::::::::::::::::::::::::::::::::

1.  Proceed to a (temporary) build directory. For example

    .. code-block:: bash

      $ cd /path/to/GooseEYE/src/build

2.  'Build' ``GooseEYE`` to install it in a custom location

    .. code-block:: bash

      $ mkdir /custom/install/path
      $ cmake .. -DCMAKE_INSTALL_PREFIX:PATH=/custom/install/path
      $ make install

    (If you've used another build directory, change the first command to ``$ cmake /path/to/GooseEYE/src``)

3.  Add the following path to your ``~/.bashrc`` (or ``~/.zshrc``):

    .. code-block:: bash

      export PKG_CONFIG_PATH=/custom/install/path/share/pkgconfig:$PKG_CONFIG_PATH

.. note:: **(Not recommended)**

  If you do not wish to use ``CMake`` for the installation, or you want to do something custom. You can of course. Follow these steps:

  1.  Copy the file ``src/GooseEYE.pc.in`` to ``GooseEYE.pc`` to some location that can be found by ``pkg_config`` (for example by adding ``export PKG_CONFIG_PATH=/path/to/GooseEYE.pc:$PKG_CONFIG_PATH`` to the ``.bashrc``).

  2.  Modify the line ``prefix=@CMAKE_INSTALL_PREFIX@`` to ``prefix=/path/to/GooseEYE``.

  3.  Modify the line ``Cflags: -I${prefix}/@INCLUDE_INSTALL_DIR@`` to ``Cflags: -I${prefix}/src``.

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

  find_package(PkgConfig)

  pkg_check_modules(GOOSEEYE REQUIRED GooseEYE)
  include_directories(${GOOSEEYE_INCLUDE_DIRS})


