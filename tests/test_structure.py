import faulthandler
import unittest

import GooseEYE as eye
import numpy as np

faulthandler.enable()


class Test_structure_1d(unittest.TestCase):
    def test_stragy_1d(self):
        """
        Implementation strategy (not a test of the implementation).
        """
        for n in [6, 7]:
            q = np.fft.fftfreq(n) * n
            e = n // 2 if n % 2 == 0 else n // 2 + 1
            s = e + 1 if n % 2 == 0 else e
            self.assertTrue(np.allclose(q[:e], np.arange(e)))
            self.assertTrue(np.allclose(np.flip(q[s:]), -np.arange(1, e)))

    def test_stragy_2d(self):
        """
        Implementation strategy (not a test of the implementation).
        """
        for n in [6, 7]:
            nrow = n
            ncol = n + 2
            qrow = np.fft.fftfreq(nrow) * nrow
            qcol = np.fft.fftfreq(ncol) * ncol

            qr = np.zeros([qrow.size, qcol.size])
            qc = np.zeros([qrow.size, qcol.size])
            for i in range(qrow.size):
                for j in range(qcol.size):
                    qr[i, j] = qrow[i]
                    qc[i, j] = qcol[j]

            erow = nrow // 2 if nrow % 2 == 0 else nrow // 2 + 1
            ecol = ncol // 2 if ncol % 2 == 0 else ncol // 2 + 1
            srow = erow + 1 if nrow % 2 == 0 else erow
            scol = ecol + 1 if ncol % 2 == 0 else ecol

            self.assertTrue(np.allclose(qr[0, 0], 0))
            self.assertTrue(np.allclose(qr[0, 1:ecol], np.flip(qr[0, scol:])))
            self.assertTrue(np.allclose(qr[1:erow, 0], -np.flip(qr[srow:, 0])))
            self.assertTrue(np.allclose(qr[1:erow, 1:ecol], -np.flip(qr[srow:, scol:])))
            self.assertTrue(np.allclose(qr[1:erow, 1:ecol], np.flip(qr[1:erow, scol:], axis=1)))
            self.assertTrue(np.allclose(qr[1:erow, 1:ecol], -np.flip(qr[srow:, 1:ecol], axis=0)))

            self.assertTrue(np.allclose(qc[0, 0], 0))
            self.assertTrue(np.allclose(qc[0, 1:ecol], -np.flip(qc[0, scol:])))
            self.assertTrue(np.allclose(qc[1:erow, 0], np.flip(qc[srow:, 0])))
            self.assertTrue(np.allclose(qc[1:erow, 1:ecol], -np.flip(qc[srow:, scol:])))
            self.assertTrue(np.allclose(qc[1:erow, 1:ecol], -np.flip(qc[1:erow, scol:], axis=1)))
            self.assertTrue(np.allclose(qc[1:erow, 1:ecol], np.flip(qc[srow:, 1:ecol], axis=0)))

    def test_qnorm_1d(self):
        n = 6
        structure = eye.Structure(shape=[n])
        qnorm = structure.qnorm * n
        self.assertTrue(np.allclose(qnorm, np.array([0, 1, 2])))

        n = 7
        structure = eye.Structure(shape=[n])
        qnorm = structure.qnorm * n
        self.assertTrue(np.allclose(qnorm, np.array([0, 1, 2, 3])))

    def test_qnorm_2d(self):
        nrow = 6
        ncol = 8
        structure = eye.Structure(shape=[nrow, ncol])
        qrow = np.array([0, 1, 2]) / nrow
        qcol = np.array([0, 1, 2, 3]) / ncol
        qnorm = np.zeros([qrow.size, qcol.size])
        for i in range(qrow.size):
            for j in range(qcol.size):
                qnorm[i, j] = np.sqrt(qrow[i] ** 2 + qcol[j] ** 2)
        self.assertTrue(np.allclose(structure.qnorm, qnorm))

        nrow = 7
        ncol = 9
        structure = eye.Structure(shape=[nrow, ncol])
        qrow = np.array([0, 1, 2, 3]) / nrow
        qcol = np.array([0, 1, 2, 3, 4]) / ncol
        qnorm = np.zeros([qrow.size, qcol.size])
        for i in range(qrow.size):
            for j in range(qcol.size):
                qnorm[i, j] = np.sqrt(qrow[i] ** 2 + qcol[j] ** 2)
        self.assertTrue(np.allclose(structure.qnorm, qnorm))

    def test_q(self):
        nrow = 6
        ncol = 8
        structure = eye.Structure(shape=[nrow, ncol])
        self.assertEqual(structure.q(0).shape, (nrow, ncol))
        self.assertEqual(structure.q(1).shape, (nrow, ncol))

    def test_even_nan_1d(self):
        n = 6
        structure = eye.Structure(shape=[n])
        self.assertTrue(np.isnan(structure.first[n // 2]))
        structure += np.zeros([n], dtype=float)
        self.assertTrue(np.isnan(structure.first[n // 2]))

        n = 7
        structure = eye.Structure(shape=[n])
        self.assertTrue(not np.any(np.isnan(structure.first)))
        structure += np.zeros([n], dtype=float)
        self.assertTrue(not np.any(np.isnan(structure.first)))

    def test_even_nan_2d(self):
        nrow = 6
        ncol = 8
        structure = eye.Structure(shape=[nrow, ncol])
        for data in [structure.first, structure.second]:
            self.assertTrue(np.all(np.isnan(data[nrow // 2, :])))
            self.assertTrue(np.all(np.isnan(data[:, ncol // 2])))
        structure += np.zeros([nrow, ncol], dtype=float)
        for data in [structure.first, structure.second]:
            self.assertTrue(np.all(np.isnan(data[nrow // 2, :])))
            self.assertTrue(np.all(np.isnan(data[:, ncol // 2])))

        nrow = 7
        ncol = 9
        structure = eye.Structure(shape=[nrow, ncol])
        for data in [structure.first, structure.second]:
            self.assertTrue(not np.any(np.isnan(data)))
        structure += np.zeros([nrow, ncol], dtype=float)
        for data in [structure.first, structure.second]:
            self.assertTrue(not np.any(np.isnan(data)))

    def test_norm_even(self):
        usum = 0
        structure = eye.Structure()

        for _ in range(1000):
            u = np.hstack((0, np.cumsum(np.random.randn(101))))
            u = u - u * np.arange(u.size) / u.size
            structure += u
            usum += np.sum((u - np.mean(u)) ** 2)

        self.assertAlmostEqual(np.sum(2 * structure.first[1 : u.size // 2]) / usum, 1, places=3)

    def test_norm_odd(self):
        usum = 0
        structure = eye.Structure()

        for _ in range(1000):
            u = np.hstack((0, np.cumsum(np.random.randn(100))))
            u = u - u * np.arange(u.size) / u.size
            structure += u
            usum += np.sum((u - np.mean(u)) ** 2)

        self.assertAlmostEqual(np.sum(structure.first[1:]) / usum, 1, places=5)

    def test_random_walk(self):
        for size in [2001, 2002]:
            structure = eye.Structure()
            for _ in range(2000):
                u = np.hstack((0, np.cumsum(np.random.randn(size))))  # random walk
                u = u - u * np.arange(u.size) / u.size  # periodic random walk
                structure += u

            q = structure.qnorm
            s = structure.mean()[1 : q.size]
            q = q[1:]
            scaling = 1 / (q**2)
            scaling *= s[1] / scaling[1]
            keep = np.logical_and(q > 1e-3, q < 1e-1)
            error = np.sqrt(np.mean((s[keep] / scaling[keep]) ** 2))
            self.assertLess(error, 1.1)
            self.assertGreater(error, 0.9)


if __name__ == "__main__":
    unittest.main(verbosity=2)
