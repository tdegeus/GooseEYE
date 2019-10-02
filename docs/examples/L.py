
# <snippet>

import GooseEYE as eye
import numpy    as np

# generate image, store 'volume-fraction'
I   = eye.dummy_circles((500,500))
phi = np.mean(I)

# lineal path function
L   = eye.L((101,101),I)

# </snippet>

# plot
# ----

import matplotlib.pyplot as plt
import matplotlib        as mpl
import matplotlib.cm     as cm
from mpl_toolkits.axes_grid1 import make_axes_locatable

try   : plt.style.use(['goose','goose-latex'])
except: pass

fig, axes = plt.subplots(figsize=(18,6), nrows=1, ncols=3)

ax = axes[0]
im = ax.imshow(I,clim=(0,1),cmap=mpl.colors.ListedColormap(cm.gray([0,255])))
ax.xaxis.set_ticks([0,500])
ax.yaxis.set_ticks([0,500])
ax.set_xlabel(r'$x$')
ax.set_ylabel(r'$y$')
ax.set_title (r'$\mathcal{I}$')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks([0,1])

ax = axes[1]
im = ax.imshow(L,clim=(0,phi),cmap='jet',extent=(-50,50,-50,50))
ax.xaxis.set_ticks([-50,0,+50])
ax.yaxis.set_ticks([-50,0,+50])
ax.set_xlabel(r'$\Delta x$')
ax.set_ylabel(r'$\Delta y$')
ax.set_title (r'$L$')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks     ([ 0 ,       phi  ])
cbar.set_ticklabels(['0',r'$\varphi$'])

ax = axes[2]
ax.plot(np.arange(-50,51),L[50,:],color='b')
ax.xaxis.set_ticks     ([-50,   0,   +50 ])
ax.yaxis.set_ticks     ([  0,       phi  ])
ax.yaxis.set_ticklabels([  0,r'$\varphi$'])
ax.set_xlim([-50,+50])
ax.set_ylim([  0,phi])
ax.set_xlabel(r'$\Delta x$')
ax.set_ylabel(r'$L$')

plt.savefig('L.svg')
