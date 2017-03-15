
# <snippet>

import gooseeye.image as gimage
import numpy          as np

# generate image
I        = np.zeros((21,21),dtype='bool')
I[ 4, 4] = True
I[14,15] = True
I[15,15] = True
I[16,15] = True
I[15,14] = True
I[15,16] = True

# clusters
C,_ = gimage.clusters(I,periodic=True)

# dilation settings:
# cluster 1 -> 1 iteration
# cluster 2 -> 2 iterations
itr = np.arange(np.max(C)+1,dtype='int32')

# dilate
CD  = gimage.dilate(C,iterations=itr,periodic=True)

# </snippet>

# plot
# ----

import matplotlib.pyplot as plt
import matplotlib        as mpl
import matplotlib.cm     as cm
from mpl_toolkits.axes_grid1 import make_axes_locatable

# color-scheme: modify such that the background is white
# N.B. for a transparent background -> 4th column == 1.
cmap       = cm.jet(range(256))
cmap[0,:3] = 1.
cmap       = mpl.colors.ListedColormap(cmap)

try   : plt.style.use('goose-latex')
except: pass

fig  = plt.figure(figsize=(18,6))
fig.set_tight_layout(True)

ax   = fig.add_subplot(1,3,1)
im   = ax.imshow(I,clim=(0,1),cmap=mpl.colors.ListedColormap(cm.gray([0,255])))
ax.xaxis.set_ticks([0,20])
ax.yaxis.set_ticks([0,20])
plt.xlabel(r'$x$')
plt.ylabel(r'$y$')
plt.title (r'image')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks([0,1])

ax   = fig.add_subplot(1,3,2)
im   = ax.imshow(C,clim=(0,np.max(C)+1),cmap=cmap)
ax.xaxis.set_ticks([0,20])
ax.yaxis.set_ticks([0,20])
plt.xlim  ([0,20])
plt.ylim  ([0,20])
plt.xlabel(r'$x$')
plt.ylabel(r'$y$')
plt.title (r'clusters')

ax   = fig.add_subplot(1,3,3)
im   = ax.imshow(CD,clim=(0,np.max(C)+1),cmap=cmap)
ax.xaxis.set_ticks([0,20])
ax.yaxis.set_ticks([0,20])
plt.xlim  ([0,20])
plt.ylim  ([0,20])
plt.xlabel(r'$x$')
plt.ylabel(r'$y$')
plt.title (r'clusters + inhomogeneous dilate')

plt.savefig('clusters_dilate.svg')
