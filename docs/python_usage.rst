###################
 Functions & Usage
###################

Syntax
======

The Python interface is mostly wrapper around the C++ classes and functions.
In general the Python and C++ codes are almost identical, with as only differences:

-  The C++ ``xt::xarray`` are ``numpy.ndarray`` in Python.
-  The syntax ``::`` in Python in simply a ``.``.

For example:

.. code:: cpp

   GooseEYE::Ensemble ensemble({51, 51});

In Python is

.. code:: python

   ensemble = GooseEYE.Ensemble([51, 51])

Python only functions
=====================

Overview
--------

.. autosummary::

   GooseEYE.Structure

Details
-------

.. automodule:: GooseEYE
    :members:
