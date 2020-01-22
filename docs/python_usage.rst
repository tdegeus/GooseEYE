*****************
Functions & Usage
*****************

The Python interface is a simple wrapper around the C++ classes and functions. In general the Python and C++ codes are almost identical, with as only differences:

*   The C++ ``xt::xarray`` are ``numpy.ndarray`` in Python.

*   The syntax ``::`` in Python in simply a ``.``.


For example:

.. code-block:: cpp

    GooseEYE::Ensemble ensemble({51, 51});

In Python is

.. code-block:: python

    ensemble = GooseEYE.Ensemble([51, 51])
