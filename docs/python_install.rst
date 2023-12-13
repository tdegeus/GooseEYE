
Getting GooseEYE
================

Using conda
^^^^^^^^^^^

The quickest (but not the most efficient!) is to use *conda* to install *GooseEYE*::

    conda install -c conda-forge python-gooseeye

.. warning::

    This package does not benefit from *xsimd* optimisation, as it is not compiled on your hardware. Therefore compiling by hand can be advantageous.

From source
^^^^^^^^^^^

Start by downloading the package::

    git checkout https://github.com/tdegeus/GooseEYE.git
    cd GooseEYE

Then, install the dependencies, for example using *conda*::

    conda env update --file environment.yaml

Finally, install the package using::

    python -m pip install . -v --no-build-isolation --no-deps

If you want to use hardware optimisations, you can use::

    SKBUILD_CONFIGURE_OPTIONS="-DUSE_SIMD=1" python -m pip install . -v --no-build-isolation --no-deps

.. note::

    Since you are compiling a C++ extension, with C++ libraries as dependencies, you cannot rely solely on *pip* to install the dependencies.
    As observed, *conda* does contain all the dependencies.
    Alternatively, you can use whatever non-supported method you prefer.
