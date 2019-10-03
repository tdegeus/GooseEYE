
.. _python:

****************
Python interface
****************

Introduction
============

The interface is the same as the :ref:`cpp`, here you merely find some comments to get you going. Start by

.. code-block:: python

    import GooseEYE

Ensemble or individual image
============================

There are two modes of using the code, using:

*   An individual image, using individual functions (e.g. ``GooseFEM.S2(...)``, ``GooseFEM.W2(...)``, etc.)

*   An ensemble of images, using the ``GooseFEM.Ensemble`` class.

.. todo::

  Reference the right example.

The individual functions are simply a wrapper around the ``GooseFEM.Ensemble`` class. The general structure for an ensemble of images is as follows:

1.   Initialize the ensemble, defining some settings of which the shape of the region-of-interest is mandatory. For example:

     .. code-block:: python

        ensemble = GooseEYE.Ensemble((51, 51))

2.  Compute the statistics by evaluating a sequence of images in the ensemble. For example:

    .. code-block:: python

        ensemble.S2(...);
        ...

3.  Evaluate the result:

    .. code-block:: python

        result = ensemble.result();

    .. note::

        The variance around the average can be obtained using ``ensemble.variance()``

    .. note::

        To obtain the raw result and normalisation use:

        .. code-block:: python

            ensemble.data_first();  # first moment : x_1   + x_2   + ...
            ensemble.data_second(); # second moment: x_1^2 + x_2^2 + ...
            ensemble.norm();        # normalisation (number of measurements)

Using the individual images wrapper, all these steps are combined in a single function call with almost the same arguments. The only limitation is the the raw data and normalization cannot be accessed.

Statistics
==========

.. note::

  The functions are available directly in the ``GooseEYE`` namespace for individual images, and as member functions of the ``Ensemble``-class.

GooseEYE.mean
-------------

The arithmetic mean.

.. note::

  An overload is available to mask certain voxels

.. seealso::

  * :download:`GooseEYE.h <../include/GooseEYE/GooseEYE.h>`
  * :download:`Ensemble_mean.hpp <../include/GooseEYE/Ensemble_mean.hpp>`

GooseEYE.S2
-----------

2-point correlation.

.. note::

  An overload is available to mask certain voxels.

.. seealso::

  * :download:`GooseEYE.h <../include/GooseEYE/GooseEYE.h>`
  * :download:`Ensemble_S2.hpp <../include/GooseEYE/Ensemble_S2.hpp>`
  * :ref:`Theory & Example <theory_S2>`.

GooseEYE.C2
-----------

2-point cluster function.

.. note::

  An overload is available to mask certain voxels.

.. seealso::

  * :download:`GooseEYE.h <../include/GooseEYE/GooseEYE.h>`
  * :download:`Ensemble_C2.hpp <../include/GooseEYE/Ensemble_C2.hpp>`
  * :ref:`Theory & Example <theory_C2>`.

GooseEYE.W2
-----------

Weighted 2-point correlation.

.. note::

  An overload is available to mask certain voxels.

.. seealso::

  * :download:`GooseEYE.h <../include/GooseEYE/GooseEYE.h>`
  * :download:`Ensemble_W2.hpp <../include/GooseEYE/Ensemble_W2.hpp>`
  * :ref:`Theory & Example <theory_W2>`.

GooseEYE.heightheight
---------------------

Height-height correlation.

.. note::

  An overload is available to mask certain voxels.

.. seealso::

  * :download:`GooseEYE.h <../include/GooseEYE/GooseEYE.h>`
  * :download:`Ensemble_heightheight.hpp <../include/GooseEYE/Ensemble_heightheight.hpp>`
  * :ref:`Theory & Example <theory_heightheight>`.

Information
===========

GooseEYE.distance
-----------------

The relative distance of each pixel of the ROI.

.. seealso::

  * :download:`GooseEYE.h <../include/GooseEYE/GooseEYE.h>`
  * :download:`GooseEYE.hpp <../include/GooseEYE/GooseEYE.hpp>`
  * :ref:`Example <theory_heightheight>`.

Generate shape
==============

GooseEYE.dummy_circles
----------------------

Create a dummy binary images of circles.

.. seealso::

  * :download:`GooseEYE.h <../include/GooseEYE/GooseEYE.h>`
  * :download:`dummy_circles.hpp <../include/GooseEYE/dummy_circles.hpp>`
  * :ref:`Example <theory_S2>`.

Miscellaneous functions
=======================

.. todo::

  Describe all other functions here.

Installation
============

.. todo::

  Describe how obtains and uses xtensor.

To compile and install one can use

.. code-block:: bash

    python setup.py build
    python setup.py install

whereby ``python`` has to be replaced with your favourite Python executable. The prerequisites can be installed using ``pip install pyxtensor pybind11`` (again replace ``pip`` with your favourite Python executable).

