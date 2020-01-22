# <snippet>
import numpy as np
import GooseEYE

# generate image
I = np.zeros((21, 21), dtype='bool')
I[4, 4] = True
I[14, 15] = True
I[15, 15] = True
I[16, 15] = True
I[15, 14] = True
I[15, 16] = True

# clusters
C = GooseEYE.Clusters(I).labels()

# dilate
CD = GooseEYE.dilate(C)
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

# color-scheme: modify such that the background is white
# N.B. for a transparent background -> 4th column == 1.
cmap = cm.jet(range(256))
cmap[0, :3] = 1.0
cmap = mpl.colors.ListedColormap(cmap)

try:
    plt.style.use(['goose', 'goose-latex'])
except:
    pass

fig, axes = plt.subplots(figsize=(18, 6), nrows=1, ncols=3)

ax = axes[0]
im = ax.imshow(I, clim=(0, 1), cmap=mpl.colors.ListedColormap(cm.gray([0, 255])))
ax.xaxis.set_ticks([0, 20])
ax.yaxis.set_ticks([0, 20])
ax.set_xlabel(r'$x$')
ax.set_ylabel(r'$y$')
ax.set_title (r'image')
div = make_axes_locatable(ax)
cax = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im, cax=cax)
cbar.set_ticks([0, 1])

ax = axes[1]
im = ax.imshow(C, clim=(0, np.max(C) + 1), cmap=cmap)
ax.xaxis.set_ticks([0, 20])
ax.yaxis.set_ticks([0, 20])
ax.set_xlim([0, 20])
ax.set_ylim([0, 20])
ax.set_xlabel(r'$x$')
ax.set_ylabel(r'$y$')
ax.set_title (r'clusters')

ax = axes[2]
im = ax.imshow(CD, clim=(0, np.max(C) + 1), cmap=cmap)
ax.xaxis.set_ticks([0, 20])
ax.yaxis.set_ticks([0, 20])
ax.set_xlim([0, 20])
ax.set_ylim([0, 20])
ax.set_xlabel(r'$x$')
ax.set_ylabel(r'$y$')
ax.set_title (r'clusters + inhomogeneous dilate')

plt.savefig('clusters_dilate.svg')
