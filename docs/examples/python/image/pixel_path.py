
import gooseeye.image    as gimage
import numpy             as np
import matplotlib.pyplot as plt

try   : plt.style.use('goose-latex')
except: pass

# open new figure
fig = plt.figure(figsize=(18,6))
fig.set_tight_layout(True)

# create a grid of points, to plot a grid (below)
grid  = (
  np.hstack(( 0.*np.ones((20,1))              , 1.*np.ones((20,1))              )).T,
  np.hstack(( np.arange(20).reshape(-1,1)/19. , np.arange(20).reshape(-1,1)/19. )).T,
)

# loop over the different algorithms
for (subplot,mode) in enumerate(['bresenham','actual','full']):

  # calculate a few pixel paths
  paths = (
    gimage.path([0,0],[ 9, 2],mode=mode),
    gimage.path([0,0],[-3, 9],mode=mode),
    gimage.path([0,0],[-8, 9],mode=mode),
    gimage.path([0,0],[-9, 0],mode=mode),
    gimage.path([0,0],[-9,-3],mode=mode),
    gimage.path([0,0],[-2,-9],mode=mode),
    gimage.path([0,0],[+9,-2],mode=mode),
  )

  # store the paths as image, for plotting
  img = np.zeros((19,19),dtype='int32')
  for i,path in enumerate(paths):
    img[path[:,0]+9,path[:,1]+9] = i+1

  # plot the paths
  ax = fig.add_subplot(1,3,subplot+1)
  ax.imshow(img,cmap='afmhot_r',extent=(0,.9999,0,.9999))
  ax.plot(grid[0],grid[1],linewidth=1.,color='k')
  ax.plot(grid[1],grid[0],linewidth=1.,color='k')
  ax.xaxis.set_ticks([])
  ax.yaxis.set_ticks([])
  plt.xlabel(r'$\Delta x$')
  plt.ylabel(r'$\Delta y$')
  plt.title (r"``%s''"%mode)

# save figure
plt.savefig('pixel_path.svg')
