
Getting GooseEYE
================

Using conda
^^^^^^^^^^^

The quickest (but not the most efficient!) is to use *conda* to install *GooseEYE*::

    conda install -c conda-forge python-gooseeye

.. warning::

    This package does not benefit from *xsimd* optimisation, as it is not compiled on your hardware. Therefore compiling by hand is recommended.

From source
^^^^^^^^^^^

Start by installing the dependencies, for example using *conda*::

    conda install -c conda-forge pyxtensor xsimd

Note that *xsimd* is optional, but recommended.

.. note::

    You can also use::

        python -m pip install pyxtensor pybind11
        
    for use without *conda*. Note that you install *xsimd* yourself in such a way that Python can find it in order to use it.

Then, download the package::

    git checkout https://github.com/tdegeus/GooseEYE.git
    cd GooseEYE

Install the package using::

    python -m pip install .

.. note::

    The following will give more readable output::

        python setup.py build
        python setup.py install
