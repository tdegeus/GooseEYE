
****************
Python interface
****************

Introduction
============

The interface is the same as the :ref:`cpp`, and details of the interface can be explored in [:download:`GooseEYE.h <../include/GooseEYE/GooseEYE.h>`] here you can find some comments to get you going. Start by

.. code-block:: python

    import GooseEYE

Ensemble or individual image
============================

There are two modes of using the code. One can compute the statistics based on:

*   An individual image.
*   An ensemble of images.

The actual computation is contained in the computation of the ensemble, around which a wrapper is provided to evaluate an individual image. The computation of the ensemble proceeds in three steps:

1.   Initialize the ensemble, defining some settings of which the shape of the region-of-interest is mandatory. For example:

     .. code-block:: python

        ensemble = GooseEYE.Ensemble((51,51))

2.  Add the statistics by evaluating the different images in the ensemble. For example:

    .. code-block:: python

        ensemble.S2(...);
        ...

3.  Evaluate the result:

    .. code-block:: python

        result = ensemble.result();

    The separate raw result and normalization are also available.

Using the individual images wrapper all these steps are combined in a single function call with almost the same arguments as the underlying ``GooseEYE.Ensemle`` functions. The only limitation is the the raw data and normalization cannot be accessed.

Statistics
==========

.. note::

  The functions are available directly in the ``GooseEYE`` namespace for individual images, and as member functions of the ``Ensemble``-class.

mean
----

The arithmetic mean. An overload is available to mask certain voxels.

S2
--

:ref:`theory_S2`. Overloads are available for ``np.int`` (binary and integer) images and ``np.flat`` images, and for masked images.

W2
--

:ref:`theory_W2`. Overloads are available for different combinations of ``np.int`` (binary and integer) images and ``np.float`` images, and for masked images.

W2c
---

Collapsed weighted correlation (see: :ref:`theory_W2`). Overloads are available for ``np.int`` (binary and integer) images and ``np.float`` images, and for masked images. To automatically compute the clusters and their centres use ``W2c_auto``.

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

Installation
============

To compile and install one can use

.. code-block:: bash

    python setup.py build
    python setup.py install

whereby ``python`` has to be replaced with your favourite Python executable. The prerequisites can be installed using ``pip install cppmat pybind11`` (again replace ``pip`` with your favourite Python executable).

