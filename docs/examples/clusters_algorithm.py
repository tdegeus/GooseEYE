import pathlib

import cppcolormap as cm
import GooseEYE as eye
import matplotlib as mpl
import matplotlib.animation as animation
import matplotlib.pyplot as plt
import numpy as np

basename = pathlib.Path(__file__).stem
root = pathlib.Path(__file__).parent.absolute()

segmenter = eye.ClusterLabeller(shape=[10, 10], periodic=True)
shape = segmenter.shape


def open_top(row, col):
    row = row % shape[0]
    col = col % shape[1]
    coor = np.array(
        [
            (col, row),
            (col, row + 1),
            (col + 1, row + 1),
            (col + 1, row),
        ]
    )
    return plt.plot(coor[:, 0] - 0.5, coor[:, 1] - 0.5, color="r", alpha=1)[0]


def open_bottom(row, col):
    row = row % shape[0]
    col = col % shape[1]
    coor = np.array(
        [
            (col, row + 1),
            (col, row),
            (col + 1, row),
            (col + 1, row + 1),
        ]
    )
    return plt.plot(coor[:, 0] - 0.5, coor[:, 1] - 0.5, color="r", alpha=1)[0]


def open_left(row, col):
    row = row % shape[0]
    col = col % shape[1]
    coor = np.array(
        [
            (col, row),
            (col + 1, row),
            (col + 1, row + 1),
            (col, row + 1),
        ]
    )
    return plt.plot(coor[:, 0] - 0.5, coor[:, 1] - 0.5, color="r", alpha=1)[0]


def open_right(row, col):
    row = row % shape[0]
    col = col % shape[1]
    coor = np.array(
        [
            (col + 1, row),
            (col, row),
            (col, row + 1),
            (col + 1, row + 1),
        ]
    )
    return plt.plot(coor[:, 0] - 0.5, coor[:, 1] - 0.5, color="r", alpha=1)[0]


sequence = [
    3,
    4,
    11,
    12,
    14,
    15,
    25,
    22,
    32,
    33,
    34,
    35,
    36,
    37,
    40,
    44,
    50,
    51,
    52,
    53,
    54,
    57,
    58,
    64,
    65,
    66,
    68,
    76,
    77,
    78,
]
background = np.bincount(sequence, minlength=np.prod(shape)).reshape(shape)
background = np.where(background != 0, -1, 0)

n = 3
cmap = np.vstack((0.9 * np.ones((1, 3)), 0.5 * np.ones((1, 3)), cm.jet(n)))
cmap = mpl.colors.ListedColormap(cmap, name="myColorMap", N=cmap.shape[0])

fig, ax = plt.subplots()
fig.patch.set_alpha(0.0)

frames = []
im = ax.imshow(background, interpolation="nearest", cmap=cmap, clim=[-1, n])
frames.append([im])

for idx in np.arange(np.prod(shape)):
    if idx in sequence:
        segmenter.add_points([idx])

    ret = []
    img = np.where(segmenter.labels != 0, segmenter.labels, background)
    im = ax.imshow(img, interpolation="nearest", cmap=cmap, clim=[-1, n])
    ret.append(im)

    row, col = np.unravel_index(idx, shape)
    ret += [
        open_top(row + 1, col),
        open_bottom(row - 1, col),
        open_left(row, col + 1),
        open_right(row, col - 1),
    ]
    frames.append(ret)

im = ax.imshow(segmenter.labels, interpolation="nearest", cmap=cmap, clim=[-1, n])
frames.append([im])

ani = animation.ArtistAnimation(fig, frames, repeat=False)
(root / f"{basename}.html").write_text(ani.to_jshtml())
