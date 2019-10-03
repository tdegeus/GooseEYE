
.. _cpp:

*************
C++ interface
*************

Introduction
============

.. note::

  GooseEYE is a research code. The best reference is the code itself: this reader just gives an overview and points in the right directions.

To use,

.. code-block:: cpp

    #include <GooseEYE/GooseEYE.h>

Everything is contained in the namespace ``GooseEYE``.

Ensemble or individual image
============================

There are two modes of using the code, using:

*   An individual image, using individual functions (e.g. ``GooseFEM::S2(...)``, ``GooseFEM::W2(...)``, etc.)

*   An ensemble of images, using the ``GooseFEM::Ensemble`` class.

.. todo::

  Reference the right example.

The individual functions are simply a wrapper around the ``GooseFEM::Ensemble`` class. The general structure for an ensemble of images is as follows:

1.   Initialize the ensemble, defining some settings of which the shape of the region-of-interest is mandatory. For example:

     .. code-block:: cpp

        GooseEYE::Ensemble ensemble({51, 51});

2.  Compute the statistics by evaluating a sequence of images in the ensemble. For example:

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

            ensemble.data_first();  // first moment : x_1   + x_2   + ...
            ensemble.data_second(); // second moment: x_1^2 + x_2^2 + ...
            ensemble.norm();        // normalisation (number of measurements)

Using the individual images wrapper, all these steps are combined in a single function call with almost the same arguments. The only limitation is the the raw data and normalization cannot be accessed.

Statistics
==========

.. note::

  The functions are available directly in the ``GooseEYE`` namespace for individual images, and as member functions of the ``Ensemble``-class.

GooseEYE::mean
--------------

The arithmetic mean.

.. note::

  An overload is available to mask certain voxels

.. seealso::

  * :download:`GooseEYE.h <../include/GooseEYE/GooseEYE.h>`
  * :download:`Ensemble_mean.hpp <../include/GooseEYE/Ensemble_mean.hpp>`

GooseEYE::S2
------------

2-point correlation.

.. note::

  An overload is available to mask certain voxels.

.. seealso::

  * :download:`GooseEYE.h <../include/GooseEYE/GooseEYE.h>`
  * :download:`Ensemble_S2.hpp <../include/GooseEYE/Ensemble_S2.hpp>`
  * :ref:`Theory & Example <theory_S2>`.

GooseEYE::C2
------------

2-point cluster function.

.. note::

  An overload is available to mask certain voxels.

.. seealso::

  * :download:`GooseEYE.h <../include/GooseEYE/GooseEYE.h>`
  * :download:`Ensemble_C2.hpp <../include/GooseEYE/Ensemble_C2.hpp>`
  * :ref:`Theory & Example <theory_C2>`.

GooseEYE::W2
------------

Weighted 2-point correlation.

.. note::

  An overload is available to mask certain voxels.

.. seealso::

  * :download:`GooseEYE.h <../include/GooseEYE/GooseEYE.h>`
  * :download:`Ensemble_W2.hpp <../include/GooseEYE/Ensemble_W2.hpp>`
  * :ref:`Theory & Example <theory_W2>`.

GooseEYE::heightheight
----------------------

Height-height correlation.

.. note::

  An overload is available to mask certain voxels.

.. seealso::

  * :download:`GooseEYE.h <../include/GooseEYE/GooseEYE.h>`
  * :download:`Ensemble_heightheight.hpp <../include/GooseEYE/Ensemble_heightheight.hpp>`
  * :ref:`Theory & Example <theory_heightheight>`.

Information
===========

GooseEYE::distance
------------------

The relative distance of each pixel of the ROI.

.. seealso::

  * :download:`GooseEYE.h <../include/GooseEYE/GooseEYE.h>`
  * :download:`GooseEYE.hpp <../include/GooseEYE/GooseEYE.hpp>`
  * :ref:`Example <theory_heightheight>`.

Generate shape
==============

GooseEYE::dummy_circles
-----------------------

Create a dummy binary images of circles.

.. seealso::

  * :download:`GooseEYE.h <../include/GooseEYE/GooseEYE.h>`
  * :download:`dummy_circles.hpp <../include/GooseEYE/dummy_circles.hpp>`
  * :ref:`Example <theory_S2>`.

Miscellaneous functions
=======================

.. todo::

  Describe all other functions here.

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

  find_package(PkgConfig)

  pkg_check_modules(GOOSEEYE REQUIRED GooseEYE)
  include_directories(${GOOSEEYE_INCLUDE_DIRS})


