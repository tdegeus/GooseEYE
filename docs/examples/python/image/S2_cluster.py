
# <snippet>

import GooseEYE.image as gimage
import numpy          as np

# generate image, store 'volume-fraction'
I    = gimage.dummy_circles((500,500))
phi  = np.mean(I)

# 2-point probability
S2,_ = gimage.S2(I,I,(101,101))

# determine clusters, based on the binary image
C,_  = gimage.clusters(I)

# 2-point cluster function
C2,_ = gimage.S2(C,C,[101,101])

# </snippet>

# plot
# ----

import matplotlib.pyplot as plt
import matplotlib        as mpl
import matplotlib.cm     as cm
from mpl_toolkits.axes_grid1 import make_axes_locatable

try   : plt.style.use(['goose','goose-latex'])
except: pass

# color-scheme: modify such that the background is white
cmap       = cm.jet(range(256))
cmap[0,:3] = 1. # N.B. for a transparent background -> 4th column == 1.
cmap       = mpl.colors.ListedColormap(cmap)

fig  = plt.figure(figsize=(18,12))
fig.set_tight_layout(True)

ax   = fig.add_subplot(2,3,1)
im   = ax.imshow(I,clim=(0,1),cmap=mpl.colors.ListedColormap(cm.gray([0,255])))
ax.xaxis.set_ticks([0,500])
ax.yaxis.set_ticks([0,500])
plt.xlabel(r'$x$')
plt.ylabel(r'$y$')
plt.title (r'$\mathcal{I}$')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks([0,1])

ax   = fig.add_subplot(2,3,2)
im   = ax.imshow(S2,clim=(0,phi),cmap='jet',extent=(-50,50,-50,50))
ax.xaxis.set_ticks([-50,0,+50])
ax.yaxis.set_ticks([-50,0,+50])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$\Delta y$')
plt.title (r'$S_2$')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks     ([ 0 ,       phi  ])
cbar.set_ticklabels(['0',r'$\varphi$'])

ax   = fig.add_subplot(2,3,3)
ax.plot([-50,50],[phi**2.,phi**2.],color='k',linestyle='--',linewidth=1)
ax.plot([-50,50],[phi    ,phi    ],color='k',linestyle='--',linewidth=1)
ax.plot(np.arange(-50,51),S2[50,:],color='b')
ax.xaxis.set_ticks     ([-50,       0      ,       +50  ])
ax.yaxis.set_ticks     ([  0,       phi**2.,       phi  ])
ax.yaxis.set_ticklabels([  0,r'$\varphi^2$',r'$\varphi$'])
plt.xlim  ([-50,+50])
plt.ylim  ([  0,phi])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$S_2$')

ax   = fig.add_subplot(2,3,4)
im   = ax.imshow(C,clim=(0,np.max(C)+1),cmap=cmap)
ax.xaxis.set_ticks([0,500])
ax.yaxis.set_ticks([0,500])
plt.xlim  ([0,500])
plt.ylim  ([0,500])
plt.xlabel(r'$x$')
plt.ylabel(r'$y$')
plt.title (r'clusters')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks([])

ax   = fig.add_subplot(2,3,5)
im   = ax.imshow(C2,clim=(0,phi),cmap='jet',extent=(-50,50,-50,50))
ax.xaxis.set_ticks([-50,0,+50])
ax.yaxis.set_ticks([-50,0,+50])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$\Delta y$')
plt.title (r'$C_2$')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks([0,phi])
cbar.set_ticklabels(['0',r'$\varphi$'])

ax   = fig.add_subplot(2,3,6)
ax.plot(np.arange(-50,51),C2[50,:],color='b')
ax.xaxis.set_ticks     ([-50,0,+50])
ax.yaxis.set_ticks     ([0,       phi  ])
ax.yaxis.set_ticklabels([0,r'$\varphi$'])
plt.xlim  ([-50,+50])
plt.ylim  ([  0,phi])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$C_2$')

plt.savefig('S2_cluster.svg')
