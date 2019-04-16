
*************************
Image manipulation tricks
*************************

Segmentation
============

Ostu's method
-------------

.. note:: **References**

  *   `Implement Otsu's thresholding in Python without using OpenCV and MATLAB <https://www.quora.com/How-do-I-implement-Otsus-thresholding-in-Python-without-using-OpenCV-and-MATLAB-1>`_
  *   `Scikit's skimage.filters.threshold_otsu <https://github.com/scikit-image/scikit-image/blob/master/skimage/filters/thresholding.py#L230>`_
  *   `Scikit's Otsu's thresholding <https://scipy-lectures.org/packages/scikit-image/auto_examples/plot_threshold.html>`_

.. image:: otsu.svg
  :width: 800px
  :align: center

[:download:`source: otsu.py <otsu.py>`]

This example uses Otsu's method to combine two sources (two different images of the same object) to get an optimal threshold. The idea is to get a weighted average of two images as follows

.. math::

  \mathcal{I}
  =
  (1 - \alpha) \mathcal{I}_a + \alpha \mathcal{I}_b

whereby :math:`\alpha` is chosen such that the separability, defined by Otsu is maximised.
