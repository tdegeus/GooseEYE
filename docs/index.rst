
GooseEYE Documentation
======================

Documentation
-------------

.. toctree::
   :maxdepth: 1

   install.rst
   quickstart.rst
   cpp.rst
   python.rst
   gui.rst

Python examples
---------------

.. note::

  A little bit of theory is included in each of the examples.

.. toctree::
   :maxdepth: 1

   examples/python/readme.rst


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

.. note::

  The GooseEYE package is relies on the components below. Each have proved to be extremely attractive from an implementation side, and are therefore worth mentioning on the landing page.

  * All implementation is in C++.

  * The Python interface is generated using `pybind11 <http://pybind11.readthedocs.io/en/master/>`_. This project is particularly worth mentioning as it is extremely easy to use, and requires almost no code. Moreover it is header-only, so it requires no installation whatsoever. One just clone from `GitHub <https://github.com/pybind/pybind11>`_ and gets on its way.

  * The GUI is build using `Qt <https://www.qt.io>`_ which is completely free professional platform, that even comes with a graphical design tool: Qt Creator. Using the latter the full GUI is designed in a graphical way. One just connects some functionality using a C++ back-end. This code is even generated and accessed from Qt Creator.

  * The plotting in the GUI is done using `QCustomPlot <http://qcustomplot.com>`_ with which professional plots, including PDF exports, can be effortlessly made in a Qt application.

  * The code itself is freely hosted on the well-know platform `GitHub <http://www.github.com>`_.

  * The documentation, that you are reading right now, is freely hosted by `Read the Docs <https://readthedocs.org>`_. Its usability is very good: it automatically collects and builds the documentation when a new version is committed to GitHub.
