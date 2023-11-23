import GooseEYE as eye
import numpy as np


def test_relabel_map():
    a = np.array(
        [
            [1, 1, 0, 0, 1],
            [1, 0, 0, 0, 0],
            [0, 0, 3, 3, 0],
            [0, 0, 3, 0, 0],
            [1, 0, 0, 0, 1],
        ]
    )

    b = a.copy()
    b = np.where(b == 1, 4, b)
    b = np.where(b == 3, 7, b)

    assert list(eye.relabel_map(a, b)) == [0, 4, 0, 7]
