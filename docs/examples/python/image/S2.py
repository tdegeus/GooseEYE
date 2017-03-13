
# <snippet>

import gooseeye.image as gimage
import numpy          as np

# binary image + correlation
# --------------------------

# generate image, store 'volume-fraction'
I       = gimage.dummy_circles((500,500),periodic=True)
phi     = np.mean(I)

# 2-point probability + apply normalization
(S2,N)  = gimage.S2(I,I,(101,101))
S2      = S2.astype(np.float)/float(N)

# gray image + correlation
# ------------------------

# convert to gray-scale image by introducing noise
Igr     = np.array(I,copy=True).astype(np.float)
Igr    += 0.1*(2.0*np.random.random(Igr.size)-1.0).reshape(Igr.shape)+0.1
Igr    /= 1.2

# 2-point correlation (auto-correlation)
(cor,N) = gimage.S2(Igr,Igr,(101,101))
cor     = cor/float(N)

# mean intensity and mean of the intensity squared (for bounds)
Iav     = np.mean(Igr)
Isq     = np.mean(Igr**2.)

# </snippet>

# plot
# ----

import matplotlib.pyplot as plt
import matplotlib        as mpl
import matplotlib.cm     as cm
from mpl_toolkits.axes_grid1 import make_axes_locatable

plt.style.use('goose-latex')

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
im   = ax.imshow(S2,clim=(0,phi),cmap='jet',extent=(-50,50,-50,50))
ax.xaxis.set_ticks([-50,0,+50])
ax.yaxis.set_ticks([-50,0,+50])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$\Delta y$')
plt.title(r'$S_2$')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks([0,phi])
cbar.set_ticklabels(['0',r'$\varphi$'])

ax   = fig.add_subplot(2,3,3)
ax.plot([-50,50],[phi**2.,phi**2.],color='k',linestyle='--',linewidth=1)
ax.plot([-50,50],[phi    ,phi    ],color='k',linestyle='--',linewidth=1)
ax.plot(np.arange(-50,51),S2[50,:],color='b')
ax.xaxis.set_ticks([-50,0,+50])
ax.yaxis.set_ticks([0,phi**2.,phi])
ax.yaxis.set_ticklabels([0,r'$\varphi^2$',r'$\varphi$'])
plt.xlim([-50,50])
plt.ylim([0,phi])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$S_2$')

ax   = fig.add_subplot(2,3,4)
im   = ax.imshow(Igr,clim=(0,1),cmap='gray')
ax.xaxis.set_ticks([0,500])
ax.yaxis.set_ticks([0,500])
plt.xlabel(r'$x$')
plt.ylabel(r'$y$')
plt.title(r'$\mathcal{I}$')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks([0,1])

ax   = fig.add_subplot(2,3,5)
im   = ax.imshow(cor,clim=(0,phi),cmap='jet',extent=(-50,50,-50,50))
ax.xaxis.set_ticks([-50,0,+50])
ax.yaxis.set_ticks([-50,0,+50])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$\Delta y$')
plt.title(r'$S_2$')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right", size="5%", pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks([0,phi])
cbar.set_ticklabels(['0',r'$\varphi$'])

ax   = fig.add_subplot(2,3,6)
ax.plot([-50,50],[Iav**2.,Iav**2.],color='k',linestyle='--',linewidth=1)
ax.plot([-50,50],[Isq    ,Isq    ],color='k',linestyle='--',linewidth=1)
ax.plot(np.arange(-50,51),cor[50,:],color='b')
ax.xaxis.set_ticks([-50,0,+50])
ax.yaxis.set_ticks([0,Iav**2.,Isq])
ax.yaxis.set_ticklabels([0,r'$\langle \mathcal{I} \rangle^2$',r'$\langle \mathcal{I}^2 \rangle$'])
plt.xlim([-50,50])
plt.ylim([0,phi])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$S_2$')

plt.savefig('S2.svg')
