import numpy as np
import matplotlib.pyplot as plt
import GooseEYE

try:
    plt.style.use(['goose', 'goose-latex'])
except:
    pass

# open new figure
fig, axes = plt.subplots(figsize=(18, 6), nrows=1, ncols=3)

# create a grid of points, to plot a grid (below)
grid = (
    np.hstack((np.zeros((20, 1)), np.ones((20, 1)))).T,
    np.hstack((np.arange(20).reshape(-1, 1)/19.0, np.arange(20).reshape(-1, 1)/19.0)).T,
)

# specify different algorithms
modes = {
    'Bresenham': GooseEYE.path_mode.Bresenham,
    'actual': GooseEYE.path_mode.actual,
    'full': GooseEYE.path_mode.full,
}

# loop over the different algorithms
for (subplot, mode) in enumerate(modes):

    # calculate a few pixel paths
    paths = (
        GooseEYE.path([0,0], [9, 2], mode=modes[mode]),
        GooseEYE.path([0,0], [-3, 9], mode=modes[mode]),
        GooseEYE.path([0,0], [-8, 9], mode=modes[mode]),
        GooseEYE.path([0,0], [-9, 0], mode=modes[mode]),
        GooseEYE.path([0,0], [-9,-3], mode=modes[mode]),
        GooseEYE.path([0,0], [-2,-9], mode=modes[mode]),
        GooseEYE.path([0,0], [+9,-2], mode=modes[mode]),
    )

    # store the paths as image, for plotting
    img = np.zeros((19,19), dtype='int')
    for i, path in enumerate(paths):
        img[path[:,0]+9, path[:,1]+9] = i+1

    # plot the paths
    ax = axes[subplot]
    ax.imshow(img, cmap='afmhot_r', extent=(0, .9999, 0, .9999))
    ax.plot(grid[0], grid[1], lw=1., c='k')
    ax.plot(grid[1], grid[0], lw=1., c='k')
    ax.xaxis.set_ticks([])
    ax.yaxis.set_ticks([])
    ax.set_xlabel(r'$\Delta x$')
    ax.set_ylabel(r'$\Delta y$')
    ax.set_title(r"``{0:s}''".format(mode))
    ax.axis('off')

plt.savefig('pixel_path.svg')
