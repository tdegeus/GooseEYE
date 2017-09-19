
# <snippet>

import GooseEYE.image as gimage
import numpy          as np

# binary image + correlation
# --------------------------

# generate image, store 'volume-fraction'
I        = gimage.dummy_circles((500,500))
phi      = np.mean(I)

# 2-point probability
S2,_     = gimage.S2(I,I,(101,101))

# artifact + (masked) correlation
# -------------------------------

# define image with artifact and the corresponding mask
mask            = np.zeros(I.shape,dtype='bool')
Ierr            = np.array(I      ,copy=True   )
mask[:150,:150] = 1
Ierr[:150,:150] = 1

# 2-point correlation on image with artifact (no mask)
S2err,_  = gimage.S2(Ierr,Ierr,(101,101))

# 2-point correlation on image with artifact, with artifact masked
S2mask,_ = gimage.S2(Ierr,Ierr,(101,101),fmask=mask,gmask=mask)

# </snippet>

# plot
# ----

import matplotlib.pyplot as plt
import matplotlib        as mpl
import matplotlib.cm     as cm
from mpl_toolkits.axes_grid1 import make_axes_locatable

try   : plt.style.use('goose-latex')
except: pass

fig  = plt.figure(figsize=(18,12))
fig.set_tight_layout(True)

ax   = fig.add_subplot(2,3,1)
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

ax   = fig.add_subplot(2,3,2)
im   = ax.imshow(Ierr,clim=(0,1),cmap=mpl.colors.ListedColormap(cm.gray([0,255])))
ax.xaxis.set_ticks([0,500])
ax.yaxis.set_ticks([0,500])
plt.xlabel(r'$x$')
plt.ylabel(r'$y$')
plt.title(r'$\mathcal{I}$ (with artifact)')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks([0,1])

ax   = fig.add_subplot(2,3,3)
im   = ax.imshow(mask,clim=(0,1),cmap=mpl.colors.ListedColormap(cm.gray([0,255])))
ax.xaxis.set_ticks([0,500])
ax.yaxis.set_ticks([0,500])
plt.xlabel(r'$x$')
plt.ylabel(r'$y$')
plt.title(r'mask')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks([0,1])

ax   = fig.add_subplot(2,3,4)
im   = ax.imshow(S2,clim=(0,phi),cmap='jet',extent=(-50,50,-50,50))
ax.xaxis.set_ticks([-50,0,+50])
ax.yaxis.set_ticks([-50,0,+50])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$\Delta y$')
plt.title(r'$S_2$')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks     ([ 0 ,       phi  ])
cbar.set_ticklabels(['0',r'$\varphi$'])

ax   = fig.add_subplot(2,3,5)
im   = ax.imshow(S2err,clim=(0,phi),cmap='jet',extent=(-50,50,-50,50))
ax.xaxis.set_ticks([-50,0,+50])
ax.yaxis.set_ticks([-50,0,+50])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$\Delta y$')
plt.title(r'$S_2$ (with artifact)')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks     ([ 0 ,       phi  ])
cbar.set_ticklabels(['0',r'$\varphi$'])

ax   = fig.add_subplot(2,3,6)
im   = ax.imshow(S2mask,clim=(0,phi),cmap='jet',extent=(-50,50,-50,50))
ax.xaxis.set_ticks([-50,0,+50])
ax.yaxis.set_ticks([-50,0,+50])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$\Delta y$')
plt.title(r'$S_2$ (masked artifact)')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks     ([ 0 ,       phi  ])
cbar.set_ticklabels(['0',r'$\varphi$'])

plt.savefig('S2_mask.svg')
