
# <snippet>

import GooseEYE.image as gimage
import numpy          as np

# binary image + correlation
# --------------------------

# generate image, store 'volume-fraction'
I      = gimage.dummy_circles((500,500))
phi    = np.mean(I)

# 2-point probability
S2,_   = gimage.S2(I,I,(101,101))

# gray image + correlation
# ------------------------

# convert to gray-scale image and introduce noise
Igr    = np.array(I,copy=True).astype(np.float)
Igr   += 0.1*(2.0*np.random.random(Igr.size)-1.0).reshape(Igr.shape)+0.1
Igr   /= 1.2

# 2-point correlation ('auto-correlation')
S2gr,_ = gimage.S2(Igr,Igr,(101,101))

# correlation bounds: mean intensity squared and mean of the intensity squared
Iav_sq = np.mean(Igr)**2.
Isq_av = np.mean(Igr**2.)

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
plt.title (r'$\mathcal{I}$ (black/white)')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right",size="5%",pad=0.1)
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
cax  = div.append_axes("right",size="5%",pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks     ([ 0 ,       phi  ])
cbar.set_ticklabels(['0',r'$\varphi$'])

ax   = fig.add_subplot(2,3,3)
ax.plot([-50,50],[phi**2.,phi**2.],color='k',linestyle='--',linewidth=1)
ax.plot(np.arange(-50,51),S2[50,:],color='b')
ax.xaxis.set_ticks     ([-50,       0      ,       +50  ])
ax.yaxis.set_ticks     ([  0,       phi**2.,       phi  ])
ax.yaxis.set_ticklabels([  0,r'$\varphi^2$',r'$\varphi$'])
plt.xlim  ([-50,+50])
plt.ylim  ([  0,phi])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$S_2$')

ax   = fig.add_subplot(2,3,4)
im   = ax.imshow(Igr,clim=(0,1),cmap='gray')
ax.xaxis.set_ticks([0,500])
ax.yaxis.set_ticks([0,500])
plt.xlabel(r'$x$')
plt.ylabel(r'$y$')
plt.title (r'$\mathcal{I}$ (gray)')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right",size="5%",pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks([0,1])

ax   = fig.add_subplot(2,3,5)
im   = ax.imshow(S2gr,clim=(0,phi),cmap='jet',extent=(-50,50,-50,50))
ax.xaxis.set_ticks([-50,0,+50])
ax.yaxis.set_ticks([-50,0,+50])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$\Delta y$')
plt.title (r'$S_2$')
div  = make_axes_locatable(ax)
cax  = div.append_axes("right",size="5%",pad=0.1)
cbar = plt.colorbar(im,cax=cax)
cbar.set_ticks([0,phi])
cbar.set_ticklabels(['0',r'$\varphi$'])

ax   = fig.add_subplot(2,3,6)
ax.plot([-50,50],[Iav_sq,Iav_sq],color='k',linestyle='--',linewidth=1)
ax.plot([-50,50],[Isq_av,Isq_av],color='k',linestyle='--',linewidth=1)
ax.plot(np.arange(-50,51),S2gr[50,:],color='b')
ax.xaxis.set_ticks     ([-50,           0                      ,           +50                                 ])
ax.yaxis.set_ticks     ([  0,           Iav_sq                 ,           Isq_av                 ,       phi  ])
ax.yaxis.set_ticklabels([  0,r'$\langle \mathcal{I} \rangle^2$',r'$\langle \mathcal{I}^2 \rangle$',r'$\varphi$'])
plt.xlim  ([-50,+50])
plt.ylim  ([  0,phi])
plt.xlabel(r'$\Delta x$')
plt.ylabel(r'$S_2$')

plt.savefig('S2.svg')
