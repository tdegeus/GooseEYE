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
        self.sample_shape = shape
        self.q = None
        super().__init__(compute_variance=compute_variance, shape=None, dtype=dtype)

    def _get_q(self):
        assert self.sample_shape is not None

        for s in self.sample_shape:
            if s % 2 != 0:
                raise NotImplementedError

        if len(self.sample_shape) == 1:
            return np.fft.fftfreq(self.sample_shape[0])[: int(self.sample_shape[0] / 2)]

        raise NotImplementedError

    def _allocate(self, shape, dtype):
        if self.sample_shape is not None:
            assert np.all(np.equal(shape, self.sample_shape))
        else:
            self.sample_shape = shape

        self.q = self._get_q()
        super()._allocate([self.q.size], dtype)

    def _check_dimensions(self):
        assert np.allclose(self.q, self._get_q())
        return super()._check_dimensions()

    def __iter__(self):
        super().__iter__()
        yield "q", self.q
        yield "sample_shape", self.sample_shape

    @classmethod
    def restore(
        cls,
        first: ArrayLike = None,
        second: ArrayLike = None,
        norm: ArrayLike = None,
        q: ArrayLike = None,
        sample_shape: tuple[int] = None,
    ):
        """
        Restore previous data.

        :param first: Continued computation: Sum of the first moment.
        :param second: Continued computation: Sum of the second moment.
        :param norm: Continued computation: Number of samples (integer).
        :param q: (Norm of) the frequency.
        :param sample_shape: Shape of the sample.
        """
        ret = cls(compute_variance=second is not None)
        ret.first = first
        ret.second = second
        ret.norm = norm
        ret.q = q
        ret.sample_shape = sample_shape
        return ret._check_dimensions()

    def add_sample(self, data: ArrayLike):
        r"""
        Add a sample.
        :param data: The sample.
        """
        if self.first is None:
            self._allocate(data.shape, data.dtype)

        assert np.all(np.equal(data.shape, self.sample_shape))
        datum = np.empty_like(self.first)

        if len(self.sample_shape) == 1:
            hat = np.fft.fft(data)
            b = int(data.size / 2)
            datum[0] = 2 * np.real(hat[0] ** 2)
            datum[1:] = 2 * np.real(hat[1:b] * np.flip(hat[b + 1 :]))

        super().add_sample(datum / hat.size)
