# <snippet>
import numpy as np
import GooseEYE

# square grid of circles
N = 15
M = 500
row = np.linspace(0, M, N)
col = np.linspace(0, M, N)
row, col = np.meshgrid(row, col)
row = row.reshape(-1)
col = col.reshape(-1)
r = float(M) / float(N) / 4.0 * np.ones((N * N))

# random perturbation
row += np.random.normal(0.0, float(M) / float(N), N * N)
col += np.random.normal(0.0, float(M) / float(N), N * N)
r *= np.random.random(N * N) * 2.0 + 0.1

# generate image
I = GooseEYE.dummy_circles((M, M),
    row.astype(np.int),
    col.astype(np.int),
    r.astype(np.int))

# get 'volume fraction'
phi = np.mean(I)

# create 'damage' -> right of inclusion
col += 1.1 * r
r *= 0.4

W = GooseEYE.dummy_circles((M, M),
    row.astype(np.int),
    col.astype(np.int),
    r.astype(np.int))

W[I == 1] = 0

# compute individual damage clusters and their centers
Clusters = GooseEYE.Clusters(W)
clusters = Clusters.labels()
centers = Clusters.centers()

# weighted correlation
W_I = GooseEYE.W2((101, 101), W, I, fmask=W)

# collapsed weighted correlation
W_I_c = GooseEYE.W2c((101, 101), clusters, centers, I, fmask=W)
# </snippet>

# skip plot with "--no-plot" command line argument
# ------------------------------------------------

import sys

if len(sys.argv) == 2:
    if sys.argv[1] == "--no-plot":
        sys.exit(0)

# plot
# ----

import matplotlib.pyplot as plt
import matplotlib as mpl
import matplotlib.cm as cm
from mpl_toolkits.axes_grid1 import make_axes_locatable

try:
    plt.style.use(['goose', 'goose-latex'])

except:
    pass

cmap = mpl.colors.ListedColormap(np.array([
    [0.0, 0.0, 0.0, 0.0],
    [1.0, 0.0, 0.0, 1.0]],dtype='float64'))

fig, axes = plt.subplots(
    figsize = (18, 6),
    nrows = 1,
    ncols = 3)

# ---

ax = axes[0]

im = ax.imshow(I,
    clim=(0, 1),
    cmap = mpl.colors.ListedColormap(cm.gray([0, 255])))

ax.imshow(W,
    clim=(0, 1),
    cmap = cmap)

ax.xaxis.set_ticks([0, 500])
ax.yaxis.set_ticks([0, 500])

ax.set_xlabel(r'$x$')
ax.set_ylabel(r'$y$')
ax.set_title(r'$\mathcal{I}$ (black/white) + $\mathcal{W}$ (red)')

div = make_axes_locatable(ax)
cax = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im, cax=cax)
cbar.set_ticks([0,1])

# ---

ax = axes[1]

im = ax.imshow(W_I - phi,
    clim = (-phi, +phi),
    cmap = 'RdBu_r',
    extent = (-50, 50, -50, 50))

ax.xaxis.set_ticks([-50, 0, +50])
ax.yaxis.set_ticks([-50, 0, +50])

ax.set_xlabel(r'$\Delta x$')
ax.set_ylabel(r'$\Delta y$')
ax.set_title(r'weighted correlation')

div = make_axes_locatable(ax)
cax = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im, cax=cax)

cbar.set_ticks([-phi, 0, +phi])
cbar.set_ticklabels([r'$-\varphi$', '0', r'$+\varphi$'])

# ---

ax = axes[2]

im = ax.imshow(W_I_c - phi,
    clim = (-phi, +phi),
    cmap = 'RdBu_r',
    extent = (-50, 50, -50, 50))

ax.xaxis.set_ticks([-50, 0, +50])
ax.yaxis.set_ticks([-50, 0, +50])

ax.set_xlabel(r'$\Delta x$')
ax.set_ylabel(r'$\Delta y$')
ax.set_title(r'collapse weights to center')

div = make_axes_locatable(ax)
cax = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im, cax=cax)

cbar.set_ticks([-phi, 0, +phi])
cbar.set_ticklabels([r'$-\varphi$', '0', r'$+\varphi$'])

plt.savefig('W2c.svg')
