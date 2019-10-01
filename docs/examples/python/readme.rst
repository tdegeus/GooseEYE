
************************
Theory & Python examples
************************

.. _theory_S2:

2-point probability / auto-correlation
======================================

Theory
------

The most basic statistic determines the typical distance over which two points (pixels, voxels, ...) are related to each other. This is best understood by considering a binary 2D image, wherein each pixel is either black or white. This is described by the following indicator function, indicating the 'greyscale' of a pixel at position :math:`\vec{x}_i`:

.. math::

  \mathcal{I}(\vec{x}_i)
  =
  \begin{cases}
    1 \quad & \mathrm{if}\;\; \vec{x}_i\; \in \mathrm{white} \\
    0 \quad & \mathrm{if}\;\; \vec{x}_i\; \in \mathrm{black} \\
  \end{cases}

The 2-point probability, :math:`S_2`, is the probability that two points, at a certain distance :math:`\Delta \vec{x}` are both white. I.e.

.. math::

  S_2 (\Delta \vec{x})
  =
  P
  \big\{
    \mathcal{I}( \vec{x}               ) = 1 ,
    \mathcal{I}( \vec{x}+\Delta\vec{x} ) = 1
  \big\}

Two limits can directly be identified. If the two points coincide, the probability that both points are white is the same as the probability that one point is white: the (volume) fraction of white, :math:`\varphi`. I.e.

.. math::

  S_2 ( || \Delta \vec{x} || = 0) = \varphi

If the two points are completely uncorrelated, when the points are far apart, each point has a probability :math:`\varphi` to be white, and thus

.. math::

  S_2 ( || \Delta \vec{x} || \rightarrow \infty) = \varphi^2

In between these extremes, :math:`S_2` decays from :math:`\varphi` towards the asymptotic value of :math:`\varphi^2`.

The greyscale generalisation is the auto-correlation corresponds to a local product:

.. math::

  S_2 ( \Delta \vec{x} )
  =
  \frac{1}{N} \sum\limits_{i=1}^N \mathcal{I} ( \vec{x}_i ) \,
  \mathcal{I} ( \vec{x}_i + \Delta \vec{x} )
  \equiv \mathcal{I} ( \vec{x} ) \star \mathcal{I} ( \vec{x} )

Along the same arguments, limit values can be obtained. In this case:

.. math::

  S_2( \Delta \vec{x} = 0 )                &= \langle \mathcal{I}^2 \rangle   \\
  S_2( \Delta \vec{x} \rightarrow \infty ) &= \langle \mathcal{I}   \rangle^2

where the brackets :math:`\langle \ldots \rangle` denotes the spatial average.

Further reading
---------------

Textbooks
^^^^^^^^^

* Torquato, S. (2002). Random Heterogeneous Materials (1st ed.). Springer, New York, NY. New York. `doi:10.1007/978-1-4757-6355-3 <http://doi.org/10.1007/978-1-4757-6355-3>`_

Example
-------

This result is based on a simple, periodic, image comprising circular white inclusions embedded in a black background. The top row shows the image and the results for the binary image: from left to right: the image, the 2-point probability :math:`S_2` in two dimensions, and a cross-section of this result in the middle of the region-of-interest along the horizontal axis. The same image and results are shown on the bottom row for a greyscale image, for which noise is added and the background and the islands are made grey.

.. image:: S2.svg
  :width: 700px

This example is based on the following code (the code used for the plotting is included in the download, whereby the matplotlib-style can be installed by installing `goosempl <http://goosempl.geus.me>`_ (for example using ``pip install goosempl``).

[:download:`S2.py <S2.py>`]

.. literalinclude:: S2.py
   :language: python
   :start-after: <snippet>
   :end-before: </snippet>

Masked correlation
------------------

This function also has the possibility to mask certain pixels. The image's mask is a binary matrix of exactly the same shape as the image. For each pixel in the mask with value ``1``, the corresponding pixel in the image is ignored. The normalisation is corrected for the reduced amount of data points, whereby the number of data points is no longer constant over the region-of-interest.

.. note::

  For non-periodic images a mask in conjunction with padding of the image can be used to incorporate the full image in the statistic. Otherwise a boundary region is skipped, reducing the amount of information from the boundary region.

.. image:: S2_mask.svg
  :width: 700px

.. literalinclude:: S2_mask.py
   :language: python
   :start-after: <snippet>
   :end-before: </snippet>

[:download:`S2_mask.py <S2_mask.py>`]

2-point cluster function
========================

Theory
------

If an image consists of isolated clusters, the 2-point cluster function can be used to quantify the probability that two points are in the same cluster. It is defined as follows:

.. math::

  C_2 (\Delta x) =
  P \big\{ \mathcal{C}(\vec{x}) = \mathcal{C}(\vec{x}+\Delta\vec{x}) \neq 0 \big\}

whereby :math:`\mathcal{C}` is an indicator with a unique non-zero index for each cluster.

Further reading
---------------

Textbooks
^^^^^^^^^

* Torquato, S. (2002). Random Heterogeneous Materials (1st ed.). Springer, New York, NY. New York. `doi:10.1007/978-1-4757-6355-3 <http://doi.org/10.1007/978-1-4757-6355-3>`_

Example
-------

.. image:: S2_cluster.svg
  :width: 700px

The 2-point cluster function can be computed with the same machinery as the 2-point probability. The former uses an integer image, with a unique integer number to label each cluster. The latter simply uses binary values.

.. literalinclude:: S2_cluster.py
   :language: python
   :start-after: <snippet>
   :end-before: </snippet>

[:download:`S2_cluster.py <S2_cluster.py>`]

.. _theory_L:

Lineal path function
====================

Theory
------

The 2-point cluster function has a first order notion of connectedness. To quantify the true connectedness along a path, the lineal path function is used. The lineal path function quantifies the probability that an entire path of pixels connecting :math:`\vec{x}_i` and :math:`\vec{x}_i + \Delta x` is in the same phase:

.. math::

  L ( \Delta x ) = P
  \big\{
    \mathcal{I}( \vec{x}                 ) = 1 ,
    \mathcal{I}( \vec{x}+\delta\vec{x}_1 ) = 1 ,
    \ldots
    \mathcal{I}( \vec{x}+\Delta\vec{x}   ) = 1
  \big\}

In practice the probability is constructed by starting from each pixel :math:`\vec{x}_i` for which :math:`\mathcal{I} ( \vec{x}_i )=1` 'walking' along a pixel path until the edge of the inclusion is reached at :math:`\vec{x}_i + \delta x_j`. The value of :math:`L` is increased for all the relative positions that have been passed along the path connecting :math:`\vec{0}` and :math:`\delta \vec{x}_j`. This is then repeated for all possible directions (with each their own path).

Two limit values can again be identified. At zero distance, the volume fraction is again found:

.. math::

  L ( \Delta \vec{x} = \vec{0} ) = \varphi

Furthermore it is highly unlikely that a path can be found through the inclusion phase to a relative position very far away. I.e.

.. math::

  L ( \Delta \vec{x} \rightarrow \infty ) = 0

An important ingredient of the computation of :math:`L` is thus the choice of the pixel paths. In GooseEYE the paths are constructed between the centre of the region of interest and each of the points on the end of the region of interest. The paths can be computed using different algorithms, illustrated below:

.. image:: pixel_path.svg
  :width: 700px

[:download:`pixel_path.py <pixel_path.py>`]

Example
-------

.. image:: L.svg
  :width: 700px

.. literalinclude:: L.py
   :language: python
   :start-after: <snippet>
   :end-before: </snippet>

[:download:`L.py <L.py>`]

.. _theory_W2:

Weighted correlation
====================

Theory
------

The weighted correlation characterised the average indicator :math:`\mathcal{I}` around high weight factor :math:`\mathcal{W}`.

Mathematically the weighted correlation reads

.. math::

  \mathcal{P} ( \Delta \vec{x} ) = \frac{
    \sum_i
    \mathcal{W}( \vec{x}_i ) \;
    \mathcal{I}( \vec{x}_i + \Delta x )
  }{
    \sum_i
    \mathcal{W}( \vec{x}_i ) \;
  }

Additionally pixels can be masked, for instance to ignore :math:`\mathcal{I}` everywhere where :math:`\mathcal{W}` is non-zero. The masked correlation reads

.. math::

  \mathcal{P} (\Delta \vec{x}) =
  \frac{
    \sum_{i}\;
    \mathcal{W} (\vec{x}_i) \;
    [ \mathcal{I} (1-\mathcal{M}) ] (\vec{x}_i + \Delta \vec{x}) \;
  }{
    \sum_{i}\;
    \mathcal{W} (\vec{x}_i) \;
    (1-\mathcal{M})\, (\vec{x}_i + \Delta \vec{x}) \;
  }

where all pixels where :math:`\mathcal{M}(\vec{x}_i) = 1` are ignored; all pixels for which :math:`\mathcal{M}(\vec{x}_i) = 0` are considered as normal.

Example
-------

.. image:: W2.svg
  :width: 700px

.. literalinclude:: W2.py
   :language: python
   :start-after: <snippet>
   :end-before: </snippet>

[:download:`W2.py <W2.py>`]

Collapse to single point
------------------------

To calculate the probability of the inclusion directly next to a weight site (i.e. the red circles in the example above and below) the 'collapsed correlation' is calculated. The distance to the edge of the site, :math:`\vec{\delta}_i` is therefore corrected for as follows:

.. math::

  \mathcal{P} (\Delta \vec{x}) =
  \frac{
    \sum_{i}\;
    \mathcal{W} (\vec{x}_i) \;
    \mathcal{I} (\vec{x}_i + \Delta \vec{x} + \vec{\delta}_i) \;
  }{
    \sum_{i}\;
    \mathcal{W} (\vec{x}_i) \;
  }

Similarly to the above, a mask may be introduced as follows:

.. math::

  \mathcal{P} (\Delta \vec{x}) =
  \frac{
    \sum_{i}\;
    \mathcal{W} (\vec{x}_i) \;
    [ \mathcal{I} (1-\mathcal{M}) ] (\vec{x}_i + \Delta \vec{x} + \vec{\delta}_i) \;
  }{
    \sum_{i}\;
    \mathcal{W} (\vec{x}_i) \;
    (1-\mathcal{M})\, (\vec{x}_i + \Delta \vec{x} + \vec{\delta}_i) \;
  }

Example
^^^^^^^

.. image:: W2c.svg
  :width: 700px

.. literalinclude:: W2c.py
   :language: python
   :start-after: <snippet>
   :end-before: </snippet>

[:download:`W2c.py <W2c.py>`]

Height-height correlation
=========================

The height-height correlation corresponds to the following:

.. math::

  \mathcal{P} (\Delta \vec{x}) =
  \sqrt{ \sum_{i} \; \left(
    z (\vec{x}_i + \Delta \vec{x}) -
    z (\vec{x}_i)
  \right)^2 }

Example
-------

.. image:: heightheight.svg
  :width: 700px

.. literalinclude:: heightheight.py
   :language: python
   :start-after: <snippet>
   :end-before: </snippet>

[:download:`heightheight.py <heightheight.py>`]

Obtain clusters
===============

Calculate clusters
------------------

.. image:: clusters.svg
  :width: 700px

.. literalinclude:: clusters.py
   :language: python
   :start-after: <snippet>
   :end-before: </snippet>

[:download:`clusters.py <clusters.py>`]

Dilate clusters (differently)
-----------------------------

.. image:: clusters_dilate.svg
  :width: 700px

.. literalinclude:: clusters_dilate.py
   :language: python
   :start-after: <snippet>
   :end-before: </snippet>

[:download:`clusters_dilate.py <clusters_dilate.py>`]
