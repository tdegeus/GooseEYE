import enstat
import numpy as np
from numpy.typing import ArrayLike
from numpy.typing import DTypeLike

from ._GooseEYE import *  # noqa: F401, F403


class Structure(enstat.static):
    """
    Compute the ensemble average structure factor:

    .. math::

        S(\vec{q}) = \langle \hat{u}(\vec{q}) \hat{u}(\vec{q}) \rangle

    Implementation
    --------------

    Thereby,

    .. math::

        \vec{q} = \sum_{i = 1}^d q_i \vec{e}_i

    The frequencies in each direction are organised as follows
    (see `docs <https://numpy.org/doc/stable/reference/generated/numpy.fft.fftfreq.html>`_):

    .. code-block:: python

        q = [0, 1, ...,   n/2-1,     -n/2, ..., -1] / n  # if n is even
        q = [0, 1, ..., (n-1)/2, -(n-1)/2, ..., -1] / n  # if n is odd

    with ``n`` the number of data points in direction ``i``.
    For example,

    *   if ``n = 6`` (even):

        .. code-block:: python

            q = [0, 1, 2, -3, -2, -1] / 6

    *   if ``n = 7`` (odd):

        .. code-block:: python

            q = [0, 1, 2, 3, -3, -2, -1] / 7

    In one dimension, computing a single sample :math:`s = \hat{u}(q) \hat{u}(-q)`
    thus corresponds to:

    *   If ``n`` is even:

        .. code-block:: python

            e = n / 2  # (e.g. 3 if n = 6)
            s[0] = uhat[0] * uhat[0]
            s[1:e] = uhat[1:e] * np.flip(uhat[e+1:])  # e.g. uhat([1, 2]) * uhat([5, 4])
            s[e+1:] = uhat[e+1:] * np.flip(uhat[1:e]) = np.flip(s[1:e])
            s[e] = np.NaN  # e.g. s[3] = np.NaN if n = 6, as q = -3 / 6 exists, but not q = 3 / 6

    *   If ``n`` is odd:

        .. code-block:: python

            e = (n - 1) / 2 + 1  # (e.g. 4 if n = 7)
            s[0] = uhat[0] * uhat[0]
            s[1:e] = uhat[1:e] * np.flip(uhat[e:])  # e.g. uhat([1, 2, 3]) * uhat([6, 5, 4])
            s[e:] = uhat[e:] * np.flip(uhat[1:e]) = np.flip(s[1:e])

    In the for the general case:

    .. code-block:: python

        e = n // 2 if n % 2 == 0 else n // 2 + 1
        s = e + 1 if n % 2 == 0 else e
        s[0] = uhat[0] * uhat[0]
        s[1:e] = uhat[1:e] * np.flip(uhat[s:])
        s[s:] = uhat[s:] * np.flip(uhat[1:e]) = np.flip(s[1:e])
        if n % 2 == 0:
            s[e] = np.NaN

    Suggested plotting
    ------------------

    *   In one dimensions:

        .. code-block:: python

            q = structure.qnorm
            s = structure.mean()[:q.size]
            ax.plot(q[1:], s[1:], marker=".")

    *   In two dimensions:

        .. code-block:: python

            q = structure.qnorm
            s = structure.mean()[:q.shape[0], :q.shape[1]]
            ax.plot(q[1:, 0], s[1:, 0], marker=".")

    :param compute_variance: Whether to compute the variance.
    :param shape: The shape of the data.
    :param dtype: The data type.
    """

    def __init__(
        self,
        compute_variance: bool = True,
        shape: tuple[int] = None,
        dtype: DTypeLike = np.float64,
    ):
        super().__init__(compute_variance=compute_variance, shape=shape, dtype=dtype)

    def _allocate(self, shape, dtype):
        super()._allocate(shape, dtype)
        dim = len(shape)
        assert dim in [1, 2]
        assert np.all(np.equal([i % 2 == 0 for i in shape], shape[0] % 2 == 0))

        if shape[0] % 2 != 0:
            return self

        if dim == 1:
            n = shape[0]
            self.first[n // 2] = np.NaN
        elif dim == 2:
            nrow, ncol = shape
            self.first[nrow // 2, :] = np.NaN
            self.first[:, ncol // 2] = np.NaN

        return self

    def q(self, axis: int = None):
        """
        The frequency (along a given axis).

        :param axis: The axis (if ``dimension > 1``).
        :return:
            Frequency (in selected direction) of each entry in
            ``first``, ``second``, ... ``mean()``, ....
        """
        if self.first.ndim == 1:
            return np.fft.fftfreq(self.first.shape[0])

        elif self.first.ndim == 2:
            if axis == 0:
                return np.fft.fftfreq(self.first.shape[0])
            elif axis == 1:
                return np.fft.fftfreq(self.first.shape[1])
            else:
                raise ValueError(f"Invalid axis: {axis}")

        raise NotImplementedError


    @property
    def qnorm(self):
        """
        Norm of the frequency of the structure factor.

        .. note::

            There are a number of duplicate entries in the raw data
            (and in the norm of the frequency).
            This property has these duplicates removed.
            To get the corresponding data or averages::

                q = structure.qnorm

                if q.dim == 1:
                    m = structure.mean()[:q.size]
                elif q.dim == 2:
                    m = structure.mean()[:q.shape[0], :q.shape[1]]

        :return: Norm of the frequency.
        """
        if self.first.ndim == 1:
            n = self.first.shape[0]
            e = n // 2 if n % 2 == 0 else n // 2 + 1
            return np.abs(np.fft.fftfreq(n)[:e])

        elif self.first.ndim == 2:
            nrow, ncol = self.first.shape
            erow = nrow // 2 if nrow % 2 == 0 else nrow // 2 + 1
            ecol = ncol // 2 if ncol % 2 == 0 else ncol // 2 + 1
            qrow, qcol = np.meshgrid(np.fft.fftfreq(nrow), np.fft.fftfreq(ncol), indexing="ij")
            return np.sqrt(qrow**2 + qcol**2)[:erow, :ecol]

        raise NotImplementedError

    def add_sample(self, data: ArrayLike):
        """
        Add a sample.
        :param data: The sample.
        """
        datum = np.empty_like(data)

        if data.ndim == 1:
            hat = np.fft.fft(data)
            n = data.shape[0]
            e = n // 2 if n % 2 == 0 else n // 2 + 1
            s = e + 1 if n % 2 == 0 else e
            datum[0] = np.real(hat[0] * hat[0])
            datum[1:e] = np.real(hat[1:e] * np.flip(hat[s:]))
            datum[s:] = np.flip(datum[1:e])

        elif data.ndim == 2:
            hat = np.fft.fft2(data)
            nrow, ncol = data.shape
            erow = nrow // 2 if nrow % 2 == 0 else nrow // 2 + 1
            ecol = ncol // 2 if ncol % 2 == 0 else ncol // 2 + 1
            srow = erow + 1 if nrow % 2 == 0 else erow
            scol = ecol + 1 if ncol % 2 == 0 else ecol
            datum[0, 0] = np.real(hat[0, 0] * hat[0, 0])
            datum[0, 1:ecol] = np.real(hat[0, 1:ecol] * np.flip(hat[0, scol:]))
            datum[1:erow, 0] = np.real(hat[1:erow, 0] * np.flip(hat[srow:, 0]))
            datum[1:erow, 1:ecol] = np.real(hat[1:erow, 1:ecol] * np.flip(hat[srow:, scol:]))
            datum[srow:, scol:] = np.flip(datum[1:erow, 1:ecol])
            datum[1:erow, scol:] = np.real(hat[1:erow, 1:ecol] * np.flip(hat[1:erow, scol:], axis=1))
            datum[srow:, 1:ecol] = np.real(hat[1:erow, 1:ecol] * np.flip(hat[srow:, 1:ecol], axis=0))

        else:
            raise NotImplementedError

        super().add_sample(datum / hat.size)
