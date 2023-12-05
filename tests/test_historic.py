import GooseEYE as eye
import numpy as np
import prrng


def test_clusters():
    img = eye.dummy_circles([30, 30], [0, 15], [0, 15], [10, 5])
    data = {
        "centers": np.array(
            [
                [0.0, 0.0],
                [3.6, 3.6],
                [3.4, 25.8],
                [15.0, 15.0],
                [25.8, 3.4],
                [25.9, 25.9],
            ]
        ),
        "sizes": [598, 73, 63, 49, 63, 54],
    }
    data_periodic = {
        "centers": np.array([[0.0, 0.0], [0.0, 0.0], [15.0, 15.0]]),
        "sizes": [598, 253, 49],
    }

    for datum, periodic in zip([data, data_periodic], [False, True]):
        labels = eye.clusters(img, periodic=periodic)
        names = np.unique(labels)[1:]
        centers = eye.labels_centers(labels, names)
        assert np.allclose(centers, datum["centers"][1:, :], atol=1e-1)
        assert np.all(np.equal(eye.labels_sizes(labels)[:, 1], datum["sizes"]))
        assert np.all(np.equal(eye.labels_sizes(labels, names), datum["sizes"][1:]))


def test_C2():
    res = np.array(
        [
            [0.217778, 0.233333, 0.244444, 0.251111, 0.244444, 0.233333, 0.217778, 0.195556],
            [0.233333, 0.255556, 0.271111, 0.277778, 0.271111, 0.255556, 0.233333, 0.208889],
            [0.244444, 0.271111, 0.295556, 0.304444, 0.295556, 0.271111, 0.244444, 0.22],
            [0.251111, 0.277778, 0.304444, 0.335556, 0.304444, 0.277778, 0.251111, 0.224444],
            [0.244444, 0.271111, 0.295556, 0.304444, 0.295556, 0.271111, 0.244444, 0.22],
            [0.233333, 0.255556, 0.271111, 0.277778, 0.271111, 0.255556, 0.233333, 0.208889],
            [0.217778, 0.233333, 0.244444, 0.251111, 0.244444, 0.233333, 0.217778, 0.195556],
            [0.195556, 0.208889, 0.22, 0.224444, 0.22, 0.208889, 0.195556, 0.18],
        ]
    )
    img = eye.dummy_circles([30, 30], [0, 15], [0, 15], [10, 5])
    labels = eye.clusters(img)
    assert np.allclose(eye.C2([8, 8], labels, labels), res)


def test_L():
    res = np.array(
        [
            [0.217778, 0.233333, 0.242222, 0.251111, 0.242222, 0.233333, 0.217778, 0.195556],
            [0.233333, 0.255556, 0.268889, 0.277778, 0.268889, 0.255556, 0.232222, 0.207778],
            [0.242222, 0.268889, 0.295556, 0.304444, 0.295556, 0.268889, 0.242222, 0.217778],
            [0.251111, 0.277778, 0.304444, 0.335556, 0.304444, 0.277778, 0.251111, 0.224444],
            [0.242222, 0.268889, 0.295556, 0.304444, 0.295556, 0.268889, 0.242222, 0.217778],
            [0.233333, 0.255556, 0.268889, 0.277778, 0.268889, 0.255556, 0.232222, 0.207778],
            [0.217778, 0.232222, 0.242222, 0.251111, 0.242222, 0.232222, 0.217778, 0.195556],
            [0.195556, 0.207778, 0.217778, 0.224444, 0.217778, 0.207778, 0.195556, 0.18],
        ]
    )
    img = eye.dummy_circles([30, 30], [0, 15], [0, 15], [10, 5])
    assert np.allclose(eye.L([8, 8], img), res)


def test_S2():
    res = np.array(
        [
            [0.217778, 0.233333, 0.244444, 0.251111, 0.244444, 0.233333, 0.217778, 0.195556],
            [0.233333, 0.255556, 0.271111, 0.277778, 0.271111, 0.255556, 0.233333, 0.208889],
            [0.244444, 0.271111, 0.295556, 0.304444, 0.295556, 0.271111, 0.244444, 0.22],
            [0.251111, 0.277778, 0.304444, 0.335556, 0.304444, 0.277778, 0.251111, 0.224444],
            [0.244444, 0.271111, 0.295556, 0.304444, 0.295556, 0.271111, 0.244444, 0.22],
            [0.233333, 0.255556, 0.271111, 0.277778, 0.271111, 0.255556, 0.233333, 0.208889],
            [0.217778, 0.233333, 0.244444, 0.251111, 0.244444, 0.233333, 0.217778, 0.195556],
            [0.195556, 0.208889, 0.22, 0.224444, 0.22, 0.208889, 0.195556, 0.18],
        ]
    )
    img = eye.dummy_circles([30, 30], [0, 15], [0, 15], [10, 5])
    assert np.allclose(eye.S2([8, 8], img, img), res)


def test_W2():
    wi = np.array(
        [
            [0.419355, 0.333333, 0.2, 0.086957, 0.04, 0.0, 0.0, 0.0],
            [0.592593, 0.5, 0.294118, 0.125, 0.0, 0.0, 0.0, 0.0],
            [0.8, 0.705882, 0.545455, 0.222222, 0.0, 0.0, 0.0, 0.0],
            [0.913043, 0.875, 0.777778, 0.0, 0.0, 0.0, 0.0, 0.0],
            [0.8, 0.705882, 0.545455, 0.222222, 0.0, 0.0, 0.0, 0.0],
            [0.592593, 0.5, 0.294118, 0.125, 0.0, 0.0, 0.0, 0.0],
            [0.419355, 0.333333, 0.2, 0.086957, 0.04, 0.0, 0.0, 0.0],
            [0.27027, 0.205882, 0.129032, 0.033333, 0.0, 0.0, 0.0, 0.025],
        ]
    )

    wic = np.array(
        [
            [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
            [0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
            [0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
        ]
    )

    img = eye.dummy_circles([30, 30], [0, 15], [0, 15], [10, 5])
    w = eye.dummy_circles([30, 30], [15], [20], [5])
    labels = eye.clusters(w)

    names = np.unique(labels)[1:]
    pos = eye.labels_centers(labels, names)
    index = np.ravel_multi_index(np.rint(pos).astype(int).T, labels.shape)
    centers = np.zeros_like(labels)
    centers.flat[index] = names

    assert np.allclose(eye.W2([8, 8], w, img, w), wi)
    assert np.allclose(eye.W2c([8, 8], labels, centers, img, w), wic)


def test_heightheight():
    L = 2 * np.pi
    N = 1000
    x = np.linspace(0, L, N)
    y = np.sin(x)
    h = [0.013326, 0.008886, 0.004445, 0.0, 0.004445, 0.008886, 0.013326, 0.017767]
    assert np.allclose(eye.heightheight([8], y), h, 1e-4)


def test_mean():
    rng = prrng.pcg32(0)
    random_data = rng.random([40, 40, 40])

    ensemble = eye.Ensemble([1], True, True)
    for i in range(random_data.shape[0]):
        ensemble.mean(random_data[i, ...])

    assert np.isclose(ensemble.result(), np.mean(random_data), 1e-4)
    assert np.isclose(ensemble.variance(), np.var(random_data), 1e-4)


def test_pixel_path():
    assert np.all(
        np.equal(
            eye.path([0, 0], [-6, 3]),
            np.array([[0, 0], [-1, 1], [-2, 1], [-3, 2], [-4, 2], [-5, 3], [-6, 3]]),
        )
    )

    assert np.all(
        np.equal(
            eye.path([0, 0], [-6, 3], eye.path_mode.Bresenham),
            np.array([[0, 0], [-1, 1], [-2, 1], [-3, 2], [-4, 2], [-5, 3], [-6, 3]]),
        )
    )

    path = np.array(
        [[0, 0], [-1, 0], [-1, 1], [-2, 1], [-3, 1], [-3, 2], [-4, 2], [-5, 2], [-5, 3], [-6, 3]]
    )
    assert np.all(np.equal(eye.path([0, 0], [-6, 3], eye.path_mode.actual), path))

    path = np.array(
        [[0, 0], [-1, 0], [-1, 1], [-2, 1], [-3, 1], [-3, 2], [-4, 2], [-5, 2], [-5, 3], [-6, 3]]
    )
    assert np.all(np.equal(eye.path([0, 0], [-6, 3], eye.path_mode.full), path))

    for mode in [eye.path_mode.Bresenham, eye.path_mode.full, eye.path_mode.actual]:
        assert np.all(np.equal(eye.path([0, 0], [0, 2], mode), [[0, 0], [0, 1], [0, 2]]))
