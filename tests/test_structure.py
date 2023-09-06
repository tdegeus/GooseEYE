import faulthandler
import unittest

import GooseEYE as eye
import numpy as np

faulthandler.enable()


class Test_structure(unittest.TestCase):
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
        structure = eye.Structure()

        for _ in range(2000):
            u = np.hstack((0, np.cumsum(np.random.randn(2001))))  # random walk
            u = u - u * np.arange(u.size) / u.size  # periodic random walk
            structure += u

        q = structure.qnorm[1:]
        s = structure.mean_norm()[1:]
        scaling = 1 / (q**2)
        scaling *= s[1] / scaling[1]
        keep = np.logical_and(q > 1e-3, q < 1e-1)
        error = np.sqrt(np.mean((s[keep] / scaling[keep]) ** 2))
        self.assertLess(error, 1.1)
        self.assertGreater(error, 0.9)


if __name__ == "__main__":
    unittest.main(verbosity=2)
