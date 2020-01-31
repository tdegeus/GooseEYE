
Paradigm
========

.. note::

    GooseEYE is a research code.
    The best reference is the code itself:
    this reader just gives an overview and points in the right directions.

Header-only
-----------

Just

.. code-block:: cpp

    #include <GooseEYE/GooseEYE.h>

Everything is contained in the namespace ``GooseEYE``.

Ensemble or individual image
----------------------------

There are two modes of using the code:

*   Individual image: use individual functions
    (e.g. ``GooseFEM::S2(...)``, ``GooseFEM::W2(...)``, etc.)

*   Ensemble of images: use the ``GooseFEM::Ensemble`` class.
    See :ref:`example <theory_S2_ensemble>`

The individual functions are simply a wrapper around the ``GooseFEM::Ensemble`` class.
The general structure for an ensemble of images is as follows:

1.  Initialize the ensemble, defining some settings of which the shape of the
    region-of-interest is mandatory. For example:

    .. code-block:: cpp

        GooseEYE::Ensemble ensemble({51, 51});

2.  Compute the statistics by evaluating a sequence of images in the ensemble. For example:

    .. code-block:: cpp

        ensemble.S2(Ia, Ia);
        ensemble.S2(Ib, Ib);
        ensemble.S2(Ic, Ic);
        ...

3.  Evaluate the result:

    .. code-block:: cpp

        auto result = ensemble.result();

    .. note::

        The variance around the average can be obtained using

        .. code-block:: cpp

            ensemble.variance();

    .. note::

        To obtain the raw result and normalisation use:

        .. code-block:: cpp

            // first moment : x_1   + x_2   + ...
            ensemble.data_first();

            // second moment: x_1^2 + x_2^2 + ...
            ensemble.data_second();

            // normalisation (number of measurements)
            ensemble.norm();

Using the individual images wrapper,
all these steps are combined in a single function call with almost the same arguments.
The only limitation is the the raw data and normalization cannot be accessed.
