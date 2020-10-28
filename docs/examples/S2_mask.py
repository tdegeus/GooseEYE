r'''
    Plot and/or check.

Usage:
    script [options]

Options:
    -s, --save      Save output for later check.
    -c, --check     Check against earlier results.
    -p, --plot      Plot.
    -h, --help      Show this help.
'''

# <snippet>
import numpy as np
import GooseEYE

# generate image, extract 'volume-fraction' for plotting
I = GooseEYE.dummy_circles((500, 500))
phi = np.mean(I)

# 2-point probability
S2 = GooseEYE.S2((101, 101), I, I)

# define image with artefact and the corresponding mask
mask = np.zeros(I.shape, dtype='bool')
Ierr = np.array(I, copy=True)
mask[:150, :150] = 1
Ierr[:150, :150] = 1

# 2-point correlation on image with artefact (no mask)
S2err = GooseEYE.S2((101, 101), Ierr, Ierr)

# 2-point correlation on image with artefact, with artefact masked
S2mask = GooseEYE.S2((101, 101), Ierr, Ierr, fmask=mask, gmask=mask)
# </snippet>

if __name__ == '__main__':

    import docopt

    args = docopt.docopt(__doc__)

    if args['--save']:

        import h5py

        with h5py.File('S2_mask.h5', 'w') as data:
            data['I'] = I
            data['Ierr'] = Ierr
            data['mask'] = mask
            data['S2'] = S2
            data['S2err'] = S2err
            data['S2mask'] = S2mask

    if args['--check']:

        import h5py

        with h5py.File('S2_mask.h5', 'r') as data:
            assert np.all(np.equal(data['I'][...], I))
            assert np.all(np.equal(data['Ierr'][...], Ierr))
            assert np.all(np.equal(data['mask'][...], mask))
            assert np.allclose(data['S2'][...], S2)
            assert np.allclose(data['S2err'][...], S2err)
            assert np.allclose(data['S2mask'][...], S2mask)

    if args['--plot']:

        import matplotlib.pyplot as plt
        import matplotlib as mpl
        import matplotlib.cm as cm
        from mpl_toolkits.axes_grid1 import make_axes_locatable

        try:
            plt.style.use(['goose', 'goose-latex'])
        except:
            pass

        fig, axes = plt.subplots(figsize=(18, 12), nrows=2, ncols=3)

        ax = axes[0, 0]
        im = ax.imshow(I, clim=(0, 1), cmap=mpl.colors.ListedColormap(cm.gray([0, 255])))
        ax.xaxis.set_ticks([0, 500])
        ax.yaxis.set_ticks([0, 500])
        ax.set_xlabel(r'$x$')
        ax.set_ylabel(r'$y$')
        ax.set_title(r'$\mathcal{I}$')
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([0, 1])

        ax = axes[0, 1]
        im = ax.imshow(Ierr, clim=(0, 1), cmap=mpl.colors.ListedColormap(cm.gray([0, 255])))
        ax.xaxis.set_ticks([0, 500])
        ax.yaxis.set_ticks([0, 500])
        ax.set_xlabel(r'$x$')
        ax.set_ylabel(r'$y$')
        ax.set_title(r'$\mathcal{I}$ (with artifact)')
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([0, 1])

        ax = axes[0, 2]
        im = ax.imshow(mask, clim=(0, 1), cmap=mpl.colors.ListedColormap(cm.gray([0, 255])))
        ax.xaxis.set_ticks([0, 500])
        ax.yaxis.set_ticks([0, 500])
        ax.set_xlabel(r'$x$')
        ax.set_ylabel(r'$y$')
        ax.set_title(r'mask')
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([0, 1])

        ax = axes[1, 0]
        im = ax.imshow(S2, clim=(0, phi), cmap='jet', extent=(-50, 50, -50, 50))
        ax.xaxis.set_ticks([-50, 0, +50])
        ax.yaxis.set_ticks([-50, 0, +50])
        ax.set_xlabel(r'$\Delta x$')
        ax.set_ylabel(r'$\Delta y$')
        ax.set_title(r'$S_2$')
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks ([0 , phi])
        cbar.set_ticklabels(['0', r'$\varphi$'])

        ax = axes[1, 1]
        im = ax.imshow(S2err, clim=(0, phi), cmap='jet', extent=(-50, 50, -50, 50))
        ax.xaxis.set_ticks([-50, 0, +50])
        ax.yaxis.set_ticks([-50, 0, +50])
        ax.set_xlabel(r'$\Delta x$')
        ax.set_ylabel(r'$\Delta y$')
        ax.set_title(r'$S_2$ (with artifact)')
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks ([0 , phi])
        cbar.set_ticklabels(['0', r'$\varphi$'])

        ax = axes[1, 2]
        im = ax.imshow(S2mask, clim=(0, phi), cmap='jet', extent=(-50, 50, -50, 50))
        ax.xaxis.set_ticks([-50, 0, +50])
        ax.yaxis.set_ticks([-50, 0, +50])
        ax.set_xlabel(r'$\Delta x$')
        ax.set_ylabel(r'$\Delta y$')
        ax.set_title(r'$S_2$ (masked artifact)')
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks ([0 , phi])
        cbar.set_ticklabels(['0', r'$\varphi$'])

        plt.savefig('S2_mask.svg')
