Compiling
=========

Getting GooseEYE
----------------

Using conda
^^^^^^^^^^^

The easiest is to use *conda* to install *GooseEYE*::

    conda install -c conda-forge gooseeye

From source
^^^^^^^^^^^

Download the package::

    git checkout https://github.com/tdegeus/GooseEYE.git
    cd GooseEYE

Install headers, *CMake* and *pkg-config* support::

    cmake .
    make install

Compiling
---------

Using CMake
^^^^^^^^^^^

The following structure can be used form ``CMakeLists.txt``:

.. code-block:: cmake

    find_package(GooseEYE REQUIRED)

    add_executable(myexec mymain.cpp)

    target_link_libraries(myexec PRIVATE
        GooseEYE
        xtensor::optimize
        xtensor::use_xsimd)

See the `documentation of xtensor <https://xtensor.readthedocs.io/en/latest/>`_ concerning optimisation.

.. note::

    There are additional targets available to expedite your ``CMakeLists.txt``:

    *   ``GooseEYE::assert``: enable GooseEYE assertions.
    *   ``GooseEYE::debug``: enable GooseEYE and xtensor assertions (slow).
    *   ``GooseEYE::compiler_warnings``: enable compiler warnings.

By hand
^^^^^^^

Presuming that the compiler is ``c++``, compile using::

    c++ -I/path/to/GooseEYE/include ...

Note that you have to take care of the *xtensor* dependency, the C++ version, optimisation, enabling *xsimd*, ...

Using pkg-config
^^^^^^^^^^^^^^^^

Find the location of the headers can be automatised using *pkg-config*::

    pkg-config --cflags GooseEYE

