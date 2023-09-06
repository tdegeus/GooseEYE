import enstat
import numpy as np
from numpy.typing import ArrayLike
from numpy.typing import DTypeLike

from ._GooseEYE import *  # noqa: F401, F403


class Structure(enstat.static):
    """
    :shape:
        The shape of the sample.
        Warning: This is not the same shape the (mean) structure factor.
    """

    def __init__(
        self,
        compute_variance: bool = True,
        shape: tuple[int] = None,
        dtype: DTypeLike = np.float64,
    ):
        super().__init__(compute_variance=compute_variance, shape=shape, dtype=dtype)

    @property
    def qnorm(self):
        """
        Norm of the frequency of the structure factor.
        """
        if self.first.ndim == 1:
            n = self.first.shape[0]
            b = n // 2
            if n % 2 != 0:
                b += 1
            return np.abs(np.fft.fftfreq(n)[:b])

        # if self.first.ndim == 2:
        #     qm, qn = np.meshgrid(
        #         np.fft.fftfreq(self.first.shape[0]),
        #         np.fft.fftfreq(self.first.shape[1]),
        #         indexing="ij",
        #     )
        #     return np.sqrt(qm**2 + qn**2)

        raise NotImplementedError

    def mean_norm(self):
        """
        Mean of the structure factor, for the norm of the frequency.
        The output is the same shape as of :py:attr:`qnorm`.
        """
        if self.norm is None:
            return None

        if self.first.ndim == 1:
            n = self.first.shape[0]
            b = n // 2
            if n % 2 != 0:
                b += 1
            norm = self.norm[0]
            assert np.all(np.equal(norm, self.norm))
            ret = 2 * self.first[:b] / norm
            ret[0] /= 2
            return ret

        raise NotImplementedError

    def add_sample(self, data: ArrayLike):
        """
        Add a sample.
        :param data: The sample.
        """
        if self.first is None:
            self._allocate(data.shape, data.dtype)
            even = data.shape[0] % 2 == 0
            assert np.all(np.equal([i % 2 == 0 for i in data.shape], data.shape[0] % 2 == 0))
            if even:
                if data.ndim == 1:
                    n = data.shape[0]
                    self.first[-int(n / 2)] = np.NaN
                elif data.ndim == 2:
                    n, m = data.shape
                    self.first[-int(n / 2), :] = np.NaN
                    self.first[:, -int(m / 2)] = np.NaN

        datum = np.empty_like(data)

        if data.ndim == 1:
            n = data.shape[0]
            hat = np.fft.fft(data)
            datum[0] = 2 * np.real(hat[0] ** 2)
            if n % 2 == 0:
                b = int(n / 2)
                datum[1:b] = np.real(hat[1:b] * np.flip(hat[b + 1 :]))
                datum[b + 1 :] = np.flip(datum[1:b])
            else:
                b = int((n - 1) / 2) + 1
                datum[1:b] = np.real(hat[1:b] * np.flip(hat[b:]))
                datum[b:] = np.flip(datum[1:b])
        else:
            raise NotImplementedError

        super().add_sample(datum / hat.size)
