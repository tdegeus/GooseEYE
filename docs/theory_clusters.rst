.. _theory_clusters:

Obtain clusters
===============

.. raw:: html
    :file: examples/clusters_algorithm.html

Calculate clusters
------------------

Extract clusters ('islands' of connected pixels with the same value).

.. image:: examples/clusters.svg
    :width: 700px

.. tabs::

    .. tab:: Python

        :download:`clusters.py <examples/clusters.py>`

        .. literalinclude:: examples/clusters.py
            :language: python
            :start-after: <snippet>
            :end-before: </snippet>

    .. tab:: C++

        :download:`clusters.cpp <examples/clusters.cpp>`

        .. literalinclude:: examples/clusters.cpp
            :language: cpp

Calculate clusters and centers
------------------------------

.. image:: examples/clusters_centers.svg
    :width: 700px

.. tabs::

    .. tab:: Python

        :download:`clusters_centers.py <examples/clusters_centers.py>`

        .. literalinclude:: examples/clusters_centers.py
            :language: python
            :start-after: <snippet>
            :end-before: </snippet>

    .. tab:: C++

        :download:`clusters_centers.cpp <examples/clusters_centers.cpp>`

        .. literalinclude:: examples/clusters_centers.cpp
            :language: cpp

Dilate clusters (differently)
-----------------------------

.. image:: examples/clusters_dilate.svg
    :width: 700px

.. tabs::

    .. tab:: Python

        :download:`clusters_dilate.py <examples/clusters_dilate.py>`

        .. literalinclude:: examples/clusters_dilate.py
            :language: python
            :start-after: <snippet>
            :end-before: </snippet>

        .. note::

            There is an additional example to show the effect of periodicity:
            :download:`clusters_dilate_periodic.py <examples/clusters_dilate_periodic.py>`
            :download:`clusters_dilate_periodic.svg <examples/clusters_dilate_periodic.svg>`

    .. tab:: C++

        :download:`clusters_dilate.cpp <examples/clusters_dilate.cpp>`

        .. literalinclude:: examples/clusters_dilate.cpp
            :language: cpp
