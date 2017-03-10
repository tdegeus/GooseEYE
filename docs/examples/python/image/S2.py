
# <snippet>

import gooseeye.image as gimage
import numpy          as np

# binary image + correlation
# --------------------------

# square grid of circles
N      = 15
M      = 500
x      = np.linspace(0,M,N)
y      = np.linspace(0,M,N)
(x,y)  = np.meshgrid(x,y)
x      = x.reshape(-1)
y      = y.reshape(-1)
r      = float(M)/float(N)/4.*np.ones((N*N))
# random perturbation
x     += np.random.normal(0.0,float(M)/float(N),N*N)
y     += np.random.normal(0.0,float(M)/float(N),N*N)
r     *= np.random.random(N*N)*2.+0.1
# generate image
I      = gimage.dummy_circles((M,M),x.astype(np.int),y.astype(np.int),r.astype(np.int),periodic=True)
# 'volume-fraction' (for bounds)
phi    = np.mean(I)

# 2-point probability + apply normalization
(S2,N) = gimage.S2(I,I,(101,101))
S2     = S2.astype(np.float)/float(N)





import matplotlib.pyplot as plt
plt.style.use('goose-latex')

fig = plt.figure(figsize=(18,6))
fig.set_tight_layout(True)

ax  = fig.add_subplot(1,3,1)

cax = ax.imshow(I,clim=(0,1),cmap='gray')

ax  = fig.add_subplot(1,3,2)

cax = ax.imshow(S2,clim=(0,phi),cmap='jet')

plt.show()






# I    = gimage.dummy_circles((500,500),periodic=True,N=(15,15),random=True)

# # 2-point probability
# S2   = gimage.correlate(I,I,(101,101),periodic=True)

# # 'volume-fraction' (for bounds)
# phi  = np.mean(I)

# # gray-scale image + correlation
# # ------------------------------

# # convert to gray-scale image by introducing noise
# Igr  = np.array(I,copy=True).astype(np.float)
# Igr += 0.1*(2.0*np.random.random(Igr.size)-1.0).reshape(Igr.shape)+0.1
# Igr /= 1.2

# # 2-point correlation (auto-correlation)
# cor  = gimage.correlate(Igr,Igr,[101,101],periodic=True)

# # mean intensity and mean of the intensity squared (for bounds)
# Iav  = np.mean(Igr)
# Isq  = np.mean(Igr**2.)

# # </snippet>

# # plot
# # ----

# # open a new figure
# gplot.figure(figsize=(15,9),fontsize=24)

# # plot image
# gplot.subplot(2,3,1)
# gplot.imshow(I,
#   clim   = (0,1),
#   cmap   = 'gray',
# )
# gplot.axes(
#   xlim   = (   0, 500),
#   ylim   = (   0, 500),
#   ntick  = (   3,   3),
#   fmt    = ('%d','%d'),
# )
# gplot.title(r'binary image: $\mathcal{I}$')

# # plot 2-point probability
# gplot.subplot(2,3,2)
# gplot.imshow(S2,
#   extent = (-50,50,-50,50),
#   clim   = (0,phi),
#   cmap   = 'jet',
# )
# gplot.axes(
#   xlim   = ( -50,  50),
#   ylim   = ( -50,  50),
#   ntick  = (   3,   3),
#   fmt    = ('%d','%d'),
# )
# gplot.title(r'2-point probability: $S_2$')

# # plot 2-point probability: horizontal cross-section
# gplot.subplot(2,3,3)
# gplot.plot(np.arange(-50,51),S2[50,:])
# gplot.plot(( -50,  50),(phi**2.0,phi**2.0),linestyle='--',linewidth=1,color='k')
# gplot.axes(
#   ylim   = (   0, phi),
#   xlim   = ( -50,  50),
#   ntick  = (   3,   0),
#   fmt    = ('%d','%d'),
# )
# gplot.axistick(
#   axis        = 'yl',
#   tick        = (       phi**2.0,       phi  ),
#   ticklabels  = (r'$\varphi^2$' ,r'$\varphi$'),
#   align_first = False,
# )
# gplot.title(r'cross section ($x$-axis)')

# # plot gray-scale image
# gplot.subplot(2,3,4)
# gplot.imshow(Igr,
#   clim  = (0,1),
#   cmap  = 'gray',
# )
# gplot.axes(
#   xlim  = (   0, 500),
#   ylim  = (   0, 500),
#   ntick = (   3,   3),
#   fmt   = ('%d','%d'),
# )
# gplot.title(r'image with noise: $\mathcal{I}$')

# # plot 2-point correlation of the gray-scale image
# gplot.subplot(2,3,5)
# gplot.imshow(cor,
#   extent = (-50,50,-50,50),
#   clim   = (0,phi),
#   cmap   = 'jet'
# )
# gplot.axes(
#   xlim  = ( -50,  50),
#   ylim  = ( -50,  50),
#   ntick = (   3,   3),
#   fmt   = ('%d','%d'),
# )
# gplot.title(r'auto-correlation: $\mathcal{I} \star \mathcal{I}$')

# # plot 2-point correlation: horizontal cross-section
# gplot.subplot(2,3,6)
# gplot.plot(np.arange(-50,51),cor[50,:])
# gplot.plot(( -50,  50),(Iav**2.,Iav**2.),linestyle='--',linewidth=1,color='k')
# gplot.plot(( -50,  50),(Isq    ,Isq    ),linestyle='--',linewidth=1,color='k')
# gplot.axes(
#   ylim   = (   0, phi),
#   xlim   = ( -50,  50),
#   ntick  = (   3,   0),
#   fmt    = ('%d','%d'),
# )
# gplot.axistick(
#   axis        = 'yl',
#   tick        = (Iav**2.0,Isq),
#   ticklabels  = (r'$\langle \mathcal{I} \rangle^2$',r'$\langle \mathcal{I}^2 \rangle$'),
#   align_first = False
# )
# gplot.title(r'cross section ($x$-axis)')

# # save figure
# gplot.savefig('correlate.svg')

