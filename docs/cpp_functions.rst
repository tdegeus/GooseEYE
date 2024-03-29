
.. _cpp_functions:

*********
Functions
*********

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

GooseEYE::W2c
-------------

Collapsed weighted 2-point correlation.

.. note::

    An overload is available to mask certain voxels.

.. seealso::

    * :download:`GooseEYE.h <../include/GooseEYE/GooseEYE.h>`
    * :download:`Ensemble_W2c.hpp <../include/GooseEYE/Ensemble_W2c.hpp>`
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

GooseEYE::clusters
------------------

Get clusters.

.. seealso::

    * :download:`GooseEYE.h <../include/GooseEYE/GooseEYE.h>`
    * :ref:`Example <theory_clusters>`.
