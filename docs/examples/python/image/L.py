
# <snippet>

import gooseeye.image as gimage
import numpy          as np

# generate image, store 'volume-fraction'
I   = gimage.dummy_circles((500,500))
phi = np.mean(I)

# lineal path function
L,_ = gimage.L(I,(101,101))

# </snippet>

# plot
# ----

import matplotlib.pyplot as plt
import matplotlib        as mpl
import matplotlib.cm     as cm
from mpl_toolkits.axes_grid1 import make_axes_locatable

try   : plt.style.use('goose-latex')
except: pass

fig  = plt.figure(figsize=(18,6))
fig.set_tight_layout(True)

ax   = fig.add_subplot(1,3,1)
im   = ax.imshow(I,clim=(0,1),cmap=mpl.colors.ListedColormap(cm.gray([0,255])))
ax.xaxis.set_ticks([0,500])
ax.yaxis.set_ticks([0,500])
plt.xlabel(r'$x$')
plt.ylabel(r'$y$')
plt.title(r'$\mathcal{I}$')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks([0,1])

ax   = fig.add_subplot(1,3,2)
im   = ax.imshow(L,clim=(0,phi),cmap='jet',extent=(-50,50,-50,50))
ax.xaxis.set_ticks([-50,0,+50])
ax.yaxis.set_ticks([-50,0,+50])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$\Delta y$')
plt.title(r'$L$')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks     ([ 0 ,       phi  ])
cbar.set_ticklabels(['0',r'$\varphi$'])

ax   = fig.add_subplot(1,3,3)
ax.plot(np.arange(-50,51),L[50,:],color='b')
ax.xaxis.set_ticks     ([-50,   0,   +50 ])
ax.yaxis.set_ticks     ([  0,       phi  ])
ax.yaxis.set_ticklabels([  0,r'$\varphi$'])
plt.xlim([-50,+50])
plt.ylim([  0,phi])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$L$')

plt.savefig('L.svg')
