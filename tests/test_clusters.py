import faulthandler

import GooseEYE as eye
import numpy as np

faulthandler.enable()


def test_init():
    s = np.zeros([4, 4], dtype=int)
    assert np.all(eye.clusters(s) == s)


def test_labels_map():
    a = np.array([[1, 1, 0, 0], [0, 0, 3, 3], [2, 2, 0, 0], [0, 0, 4, 4]])
    b = np.array([[-3, -3, 0, 0], [0, 0, 1, 1], [5, 5, 0, 0], [0, 0, 7, 7]])
    lmap = [[0, 0], [1, -3], [2, 5], [3, 1], [4, 7]]
    assert np.all(np.equal(eye.labels_map(a, b), lmap))
    assert np.all(np.equal(eye.labels_rename(a, lmap), b))


def test_simple():
    tests = [
        [
            np.array([[1, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0]]),
            np.array([[1, 1, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0]]),
            np.array([[1, 1, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0]]),
            np.array([[1, 1, 0, 0], [1, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0]]),
            np.array([[1, 1, 0, 0], [1, 0, 0, 0], [0, 0, 2, 0], [0, 0, 0, 0]]),
            np.array([[1, 1, 0, 0], [1, 0, 0, 0], [0, 0, 2, 2], [0, 0, 0, 0]]),
            np.array([[1, 1, 0, 0], [1, 0, 2, 0], [0, 0, 2, 2], [0, 0, 0, 0]]),
            np.array([[1, 1, 0, 0], [1, 1, 1, 0], [0, 0, 1, 1], [0, 0, 0, 0]]),
            np.array([[1, 1, 0, 0], [1, 1, 1, 0], [0, 0, 1, 1], [0, 0, 0, 0]]),
            np.array([[1, 1, 0, 0], [1, 1, 1, 0], [0, 0, 1, 1], [0, 0, 0, 1]]),
            np.array([[1, 1, 0, 0], [1, 1, 1, 0], [0, 0, 1, 1], [0, 0, 1, 1]]),
        ],
        [
            np.array([[1, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0]]),
            np.array([[1, 1, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0]]),
            np.array([[1, 1, 0, 0], [0, 0, 0, 0], [2, 0, 0, 0], [0, 0, 0, 0]]),
            np.array([[1, 1, 0, 0], [0, 0, 0, 0], [2, 2, 0, 0], [0, 0, 0, 0]]),
            np.array([[1, 1, 0, 0], [0, 0, 3, 0], [2, 2, 0, 0], [0, 0, 0, 0]]),
            np.array([[1, 1, 0, 0], [0, 0, 3, 3], [2, 2, 0, 0], [0, 0, 0, 0]]),
            np.array([[1, 1, 0, 0], [0, 0, 3, 3], [2, 2, 0, 0], [0, 0, 4, 0]]),
            np.array([[1, 1, 0, 0], [0, 0, 3, 3], [2, 2, 0, 0], [0, 0, 4, 4]]),
            np.array([[1, 1, 0, 1], [0, 0, 1, 1], [2, 2, 0, 0], [0, 0, 1, 1]]),
            np.array([[1, 1, 0, 1], [1, 0, 1, 1], [1, 1, 0, 0], [0, 0, 1, 1]]),
        ],
        [
            np.array([[1, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0]]),
            np.array([[1, 0, 2, 0], [0, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0]]),
            np.array([[1, 0, 2, 0], [0, 3, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0]]),
            np.array([[1, 0, 2, 0], [0, 3, 0, 4], [0, 0, 0, 0], [0, 0, 0, 0]]),
            np.array([[1, 0, 2, 0], [0, 3, 0, 4], [5, 0, 0, 0], [0, 0, 0, 0]]),
            np.array([[1, 0, 2, 0], [0, 3, 0, 4], [5, 0, 6, 0], [0, 0, 0, 0]]),
            np.array([[1, 0, 2, 0], [1, 1, 0, 1], [1, 0, 6, 0], [0, 0, 0, 0]]),
            np.array([[1, 0, 1, 0], [1, 1, 1, 1], [1, 0, 1, 0], [0, 0, 0, 0]]),
        ],
    ]

    for test in [i for test in tests for i in test]:
        labels = eye.clusters(np.where(test > 0, 1, 0), periodic=True)
        remap = eye.relabel_map(test, labels)
        assert np.all(np.equal(remap[test], labels))

    segmenter = eye.ClusterLabeller2(shape=(4, 4))
    for test in tests:
        segmenter.reset()
        for i in test:
            segmenter.add_image(np.where(i > 0, 1, 0))
            assert np.all(np.equal(segmenter.labels, i))


def test_reorder():
    labels = np.array([[1, 0, 2, 0], [0, 0, 0, 0], [3, 0, 4, 0], [0, 0, 0, 0]])
    assert np.all(np.equal(eye.clusters(np.where(labels > 0, 1, 0)), labels))

    labels = eye.labels_reorder(labels, [0, 4, 1, 2, 3])
    assert np.all(
        np.equal(labels, np.array([[2, 0, 3, 0], [0, 0, 0, 0], [4, 0, 1, 0], [0, 0, 0, 0]]))
    )


def test_rename():
    labels = np.array([[1, 0, 2, 0], [0, 0, 0, 0], [3, 0, 4, 0], [0, 0, 0, 0]])
    assert np.all(np.equal(eye.clusters(np.where(labels > 0, 1, 0)), labels))


# def test_change_label_a():
#     idx = [0, 2, 8, 10]
#     t = np.arange(len(idx)) + 1
#     c = eye.allocate_AvalancheSegmenter(shape=(4, 4), idx=idx, t=t)
#     c.advance(len(idx))
#     self.assertTrue(
#         np.all(c.s == np.array([[1, 0, 1, 0], [0, 0, 0, 0], [1, 0, 1, 0], [0, 0, 0, 0]]))
#     )
#     self.assertTrue(
#         np.all(c.labels == )
#     )
#     self.assertEqual(c.nlabels, 5)

#     c.change_labels([0, 4, 1, 2, 3])
#     self.assertTrue(
#         np.all(c.s == np.array([[1, 0, 1, 0], [0, 0, 0, 0], [1, 0, 1, 0], [0, 0, 0, 0]]))
#     )
#     self.assertTrue(
#         np.all(c.labels == np.array([[4, 0, 1, 0], [0, 0, 0, 0], [2, 0, 3, 0], [0, 0, 0, 0]]))
#     )
#     self.assertEqual(c.nlabels, 5)

# def test_change_label_b():
#     idx = [0, 2, 8, 10]
#     t = np.arange(len(idx)) + 1
#     c = eye.allocate_AvalancheSegmenter(shape=(4, 4), idx=idx, t=t)
#     c.advance(len(idx))
#     self.assertTrue(
#         np.all(c.s == np.array([[1, 0, 1, 0], [0, 0, 0, 0], [1, 0, 1, 0], [0, 0, 0, 0]]))
#     )
#     self.assertTrue(
#         np.all(c.labels == np.array([[1, 0, 2, 0], [0, 0, 0, 0], [3, 0, 4, 0], [0, 0, 0, 0]]))
#     )
#     self.assertEqual(c.nlabels, 5)

#     c.change_labels([0, 6, 1, 2, 3])
#     self.assertTrue(
#         np.all(c.s == np.array([[1, 0, 1, 0], [0, 0, 0, 0], [1, 0, 1, 0], [0, 0, 0, 0]]))
#     )
#     self.assertTrue(
#         np.all(c.labels == np.array([[6, 0, 1, 0], [0, 0, 0, 0], [2, 0, 3, 0], [0, 0, 0, 0]]))
#     )
#     self.assertEqual(c.nlabels, 7)

#     with self.assertRaises(IndexError):
#         c.change_labels([0, 1, 2, 3, 4])

#     with self.assertRaises(IndexError):
#         c.reorder([0, 7, 1, 3, 2])

#     with self.assertRaises(IndexError):
#         c.reorder([0, 5, 1, 3, 2])

#     with self.assertRaises(IndexError):
#         c.reorder([0, 6, 3, 2])

#     c.reorder([0, 6, 1, 3, 2])
#     self.assertTrue(
#         np.all(c.s == np.array([[1, 0, 1, 0], [0, 0, 0, 0], [1, 0, 1, 0], [0, 0, 0, 0]]))
#     )
#     self.assertTrue(
#         np.all(c.labels == np.array([[1, 0, 2, 0], [0, 0, 0, 0], [4, 0, 3, 0], [0, 0, 0, 0]]))
#     )
#     self.assertEqual(c.nlabels, 5)

#     c.prune()
#     c.prune()
#     self.assertTrue(
#         np.all(c.s == np.array([[1, 0, 1, 0], [0, 0, 0, 0], [1, 0, 1, 0], [0, 0, 0, 0]]))
#     )
#     self.assertTrue(
#         np.all(c.labels == np.array([[1, 0, 2, 0], [0, 0, 0, 0], [4, 0, 3, 0], [0, 0, 0, 0]]))
#     )
#     self.assertEqual(c.nlabels, 5)

# def test_change_label_c():
#     idx = [0, 2, 8, 10]
#     t = np.arange(len(idx)) + 1
#     c = eye.allocate_AvalancheSegmenter(shape=(4, 4), idx=idx, t=t)
#     c.advance(len(idx))
#     self.assertTrue(
#         np.all(c.s == np.array([[1, 0, 1, 0], [0, 0, 0, 0], [1, 0, 1, 0], [0, 0, 0, 0]]))
#     )
#     self.assertTrue(
#         np.all(c.labels == np.array([[1, 0, 2, 0], [0, 0, 0, 0], [3, 0, 4, 0], [0, 0, 0, 0]]))
#     )
#     self.assertEqual(c.nlabels, 5)

#     c.change_labels([0, 6, 1, 2, 3])
#     self.assertTrue(
#         np.all(c.s == np.array([[1, 0, 1, 0], [0, 0, 0, 0], [1, 0, 1, 0], [0, 0, 0, 0]]))
#     )
#     self.assertTrue(
#         np.all(c.labels == np.array([[6, 0, 1, 0], [0, 0, 0, 0], [2, 0, 3, 0], [0, 0, 0, 0]]))
#     )
#     self.assertEqual(c.nlabels, 7)

#     c.prune()
#     c.prune()
#     self.assertTrue(
#         np.all(c.s == np.array([[1, 0, 1, 0], [0, 0, 0, 0], [1, 0, 1, 0], [0, 0, 0, 0]]))
#     )
#     self.assertTrue(
#         np.all(c.labels == np.array([[4, 0, 1, 0], [0, 0, 0, 0], [2, 0, 3, 0], [0, 0, 0, 0]]))
#     )
#     self.assertEqual(c.nlabels, 5)

# def test_prune():
#     idx = [0, 2, 8, 10, 1]
#     t = np.arange(len(idx)) + 1
#     c = eye.allocate_AvalancheSegmenter(shape=(4, 4), idx=idx, t=t)
#     c.advance(len(idx))
#     self.assertTrue(
#         np.all(c.s == np.array([[1, 1, 1, 0], [0, 0, 0, 0], [1, 0, 1, 0], [0, 0, 0, 0]]))
#     )
#     self.assertTrue(
#         np.all(c.labels == np.array([[1, 1, 1, 0], [0, 0, 0, 0], [3, 0, 4, 0], [0, 0, 0, 0]]))
#     )
#     self.assertEqual(c.nlabels, 5)

#     c.prune()
#     self.assertTrue(
#         np.all(c.s == np.array([[1, 1, 1, 0], [0, 0, 0, 0], [1, 0, 1, 0], [0, 0, 0, 0]]))
#     )
#     self.assertTrue(
#         np.all(c.labels == np.array([[1, 1, 1, 0], [0, 0, 0, 0], [2, 0, 3, 0], [0, 0, 0, 0]]))
#     )
#     self.assertEqual(c.nlabels, 4)

# def test_bug_a():
#     shape = (10, 10)
#     actions = [
#         dict(idx=19, label=1),
#         dict(idx=5, label=2),
#         dict(idx=10, label=1),
#         dict(idx=69, label=3),
#         dict(idx=68, label=3),
#         dict(idx=24, label=4),
#         dict(idx=22, label=5),
#         dict(idx=87, label=6),
#         dict(idx=47, label=7),
#         dict(idx=8, label=8),
#         dict(idx=35, label=9),
#         dict(idx=7, label=8),
#         dict(idx=66, label=10),
#         dict(idx=92, label=11),
#         dict(idx=56, label=10),
#         dict(idx=27, label=12),
#         dict(idx=46, label=10, merge=[7, 10]),
#         dict(idx=55, label=7),
#         dict(idx=27, label=12),
#         dict(idx=37, label=7, merge=[7, 12]),
#         dict(idx=45, label=7, merge=[7, 9]),
#         dict(idx=9, label=1, merge=[1, 8]),
#         dict(idx=46, label=7),
#         dict(idx=59, label=3),
#         dict(idx=58, label=3),
#         dict(idx=48, label=3, merge=[3, 7]),
#         dict(idx=67, label=3),
#         dict(idx=36, label=3),
#         dict(idx=26, label=3),
#         dict(idx=97, label=1, merge=[1, 6]),
#         dict(idx=38, label=3),
#         dict(idx=17, label=1, merge=[1, 3]),
#         dict(idx=98, label=1),
#         dict(idx=49, label=1),
#         dict(idx=12, label=5),
#         dict(idx=99, label=1),
#         dict(idx=8, label=1),
#         dict(idx=6, label=1, merge=[1, 2]),
#         dict(idx=2, label=5, merge=[5, 11]),
#         dict(idx=94, label=13),
#         dict(idx=57, label=1),
#         dict(idx=3, label=5),
#         dict(idx=16, label=1),
#         dict(idx=0, label=1),
#         dict(idx=18, label=1),
#         dict(idx=1, label=1, merge=[1, 5]),
#         dict(idx=25, label=1, merge=[1, 4]),
#         dict(idx=64, label=14),
#         dict(idx=91, label=1),
#         dict(idx=4, label=1, merge=[1, 13]),
#         dict(idx=13, label=1),
#         dict(idx=84, label=1),
#         dict(idx=44, label=1),
#         dict(idx=65, label=1, merge=[1, 14]),
#     ]

#     idx = [action["idx"] for action in actions]
#     t = np.arange(len(idx))
#     sizes = [np.zeros(shape, dtype=int) for _ in range(len(actions))]
#     labels = [np.zeros(shape, dtype=int) for _ in range(len(actions))]

#     for i, action in enumerate(actions):
#         if i > 0:
#             sizes[i] = sizes[i - 1].copy()
#             labels[i] = labels[i - 1].copy()
#         sizes[i].flat[action["idx"]] += 1
#         labels[i].flat[action["idx"]] = action["label"]
#         if "merge" in action:
#             for label in action["merge"][1:]:
#                 labels[i] = np.where(labels[i] == label, action["merge"][0], labels[i])

#     n = len(idx)
#     for steps in range(1, n):
#         segmenter = eye.allocate_AvalancheSegmenter(shape=shape, idx=idx, t=t)
#         for i in range(n // steps + 1):
#             step = min((i + 1) * steps, n)
#             segmenter.advance(steps)
#             self.assertEqual(segmenter.nstep, step)
#             self.assertAlmostEqual(segmenter.t, t[step - 1])
#             self.assertTrue(np.all(segmenter.s == sizes[step - 1]))
#             self.assertTrue(np.all(segmenter.labels == labels[step - 1]))


def test_labels_sizes():
    labels = np.array([[1, 1, 0, 0], [1, 0, 0, 0], [0, 0, 2, 0], [0, 0, 0, 0]])
    sizes = []
    for i in range(np.max(labels) + 1):
        sizes.append(np.sum(labels == i))
    assert list(eye.labels_sizes(labels)) == sizes
