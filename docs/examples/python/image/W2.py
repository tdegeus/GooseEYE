
# <snippet>

import gooseeye.image as gimage
import numpy          as np

# image + "damage" + correlation
# ------------------------------

# square grid of circles
N         = 15
M         = 500
row       = np.linspace(0,M,N)
col       = np.linspace(0,M,N)
(row,col) = np.meshgrid(row,col)
row       = row.reshape(-1)
col       = col.reshape(-1)
r         = float(M)/float(N)/4.*np.ones((N*N))
# random perturbation
row      += np.random.normal(0.0,float(M)/float(N),N*N)
col      += np.random.normal(0.0,float(M)/float(N),N*N)
r        *= np.random.random(N*N)*2.+0.1
# generate image, store 'volume-fraction'
I         = gimage.dummy_circles((M,M),row.astype(np.int),col.astype(np.int),r.astype(np.int))
phi       = np.mean(I)

# create 'damage' -> right of inclusion
col      += 1.1*r
r        *= 0.4
W         = gimage.dummy_circles((M,M),row.astype(np.int),col.astype(np.int),r.astype(np.int))
W[I==1]   = 0

# weighted correlation
WI,_      = gimage.W2(W,I,(101,101),mask=W)

# gray-scale image + correlation
# ------------------------------

# convert to gray-scale image and introduce noise
Igr       = np.array(I,copy=True).astype(np.float)
Igr      += 0.1*(2.0*np.random.random(Igr.size)-1.0).reshape(Igr.shape)+0.1
Igr      /= 1.2
# mean intensity (for bounds)
Iav       = np.mean(Igr)

# weighted correlation
WIgr,_    = gimage.W2(W,Igr,(101,101),mask=W)


# </snippet>

# plot
# ----

import matplotlib.pyplot as plt
import matplotlib        as mpl
import matplotlib.cm     as cm
from mpl_toolkits.axes_grid1 import make_axes_locatable

try   : plt.style.use('goose-latex')
except: pass

cmap = mpl.colors.ListedColormap(np.array([
  [0.,0.,0.,0.],
  [1.,0.,0.,1.],
],dtype='float64'))

fig  = plt.figure(figsize=(18,12))
fig.set_tight_layout(True)

ax   = fig.add_subplot(2,3,1)
im   = ax.imshow(I,clim=(0,1),cmap=mpl.colors.ListedColormap(cm.gray([0,255])))
D    = ax.imshow(W,clim=(0,1),cmap=cmap)
ax.xaxis.set_ticks([0,500])
ax.yaxis.set_ticks([0,500])
plt.xlabel(r'$x$')
plt.ylabel(r'$y$')
plt.title (r'$\mathcal{I}$ (black/white) + $\mathcal{W}$ (red)')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks([0,1])

ax   = fig.add_subplot(2,3,2)
im   = ax.imshow(WI,clim=(0,1),cmap='jet',extent=(-50,50,-50,50))
ax.xaxis.set_ticks([-50,0,+50])
ax.yaxis.set_ticks([-50,0,+50])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$\Delta y$')
plt.title (r'$W_2$')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks     ([ 0 , 1 ])
cbar.set_ticklabels(['0','1'])

ax   = fig.add_subplot(2,3,3)
im   = ax.imshow(WI-phi,clim=(-phi,+phi),cmap='RdBu_r',extent=(-50,50,-50,50))
ax.xaxis.set_ticks([-50,0,+50])
ax.yaxis.set_ticks([-50,0,+50])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$\Delta y$')
plt.title (r'$W_2 - \varphi$')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks     ([       -phi  , 0 ,       +phi  ])
cbar.set_ticklabels([r'$-\varphi$','0',r'$+\varphi$'])

ax   = fig.add_subplot(2,3,4)
im   = ax.imshow(Igr,clim=(0,1),cmap='gray')
D    = ax.imshow(W  ,clim=(0,1),cmap=cmap)
ax.xaxis.set_ticks([0,500])
ax.yaxis.set_ticks([0,500])
plt.xlabel(r'$x$')
plt.ylabel(r'$y$')
plt.title (r'$\mathcal{I}$ (gray) + $\mathcal{W}$ (red)')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks([0,1])

ax   = fig.add_subplot(2,3,5)
im   = ax.imshow(WIgr,clim=(0,1),cmap='jet',extent=(-50,50,-50,50))
ax.xaxis.set_ticks([-50,0,+50])
ax.yaxis.set_ticks([-50,0,+50])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$\Delta y$')
plt.title (r'$W_2$')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks([0,1])
cbar.set_ticklabels(['0','1'])

ax   = fig.add_subplot(2,3,6)
im   = ax.imshow(WIgr-Iav,clim=(-Iav,+Iav),cmap='RdBu_r',extent=(-50,50,-50,50))
ax.xaxis.set_ticks([-50,0,+50])
ax.yaxis.set_ticks([-50,0,+50])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$\Delta y$')
plt.title (r'$W_2 - \langle \mathcal{I} \rangle$')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks     ([            -Iav                 , 0 ,            +Iav                 ])
cbar.set_ticklabels([r'$-\langle \mathcal{I} \rangle$','0',r'$+\langle \mathcal{I} \rangle$'])

plt.savefig('W2.svg')
