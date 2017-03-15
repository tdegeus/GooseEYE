
# <snippet>

import gooseeye.image as gimage
import numpy          as np

# generate image
I = gimage.dummy_circles((500,500))

# clusters, centers of gravity
# (both accounting for the periodicity of the image and ignoring it)
C ,ctr  = gimage.clusters(I,periodic=False)
CP,ctrP = gimage.clusters(I,periodic=True )
size    = np.bincount(C .ravel()).astype(np.float64)/float(C.size)
sizeP   = np.bincount(CP.ravel()).astype(np.float64)/float(C.size)

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
ax.xaxis.set_ticks([0,500])
ax.yaxis.set_ticks([0,500])
plt.xlim  ([0,500])
plt.ylim  ([0,500])
plt.xlabel(r'$x$')
plt.ylabel(r'$y$')
plt.title (r'image')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks([0,1])

ax   = fig.add_subplot(1,3,2)
im   = ax.imshow(C,clim=(0,np.max(C)+1),cmap=cmap)
idx,jdx = np.where(ctr>0)
for i,j in zip(idx,jdx):
  ax.plot(j,i,color='k',marker='+',linestyle='none',markersize=2000.*size[ctr[i,j]])
ax.xaxis.set_ticks([0,500])
ax.yaxis.set_ticks([0,500])
plt.xlim  ([0,500])
plt.ylim  ([0,500])
plt.xlabel(r'$x$')
plt.ylabel(r'$y$')
plt.title (r'clusters + centers')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks([])

ax   = fig.add_subplot(1,3,3)
im   = ax.imshow(CP,clim=(0,np.max(C)+1),cmap=cmap)
idx,jdx = np.where(ctrP>0)
for i,j in zip(idx,jdx):
  ax.plot(j,i,color='k',marker='+',linestyle='none',markersize=2000.*sizeP[ctrP[i,j]])
ax.xaxis.set_ticks([0,500])
ax.yaxis.set_ticks([0,500])
plt.xlim  ([0,500])
plt.ylim  ([0,500])
plt.xlabel(r'$x$')
plt.ylabel(r'$y$')
plt.title (r'clusters + centers (periodic)')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks([])

plt.savefig('clusters.svg')
