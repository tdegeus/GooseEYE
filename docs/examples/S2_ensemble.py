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

ensemble = GooseEYE.Ensemble((101, 101))

for i in range(5):
    I = GooseEYE.dummy_circles((200, 200))
    ensemble.S2(I, I)

S2 = ensemble.result()
# </snippet>

if __name__ == '__main__':

    import docopt

    args = docopt.docopt(__doc__)

    if args['--save']:

        import h5py

        with h5py.File('S2_ensemble.h5', 'w') as data:
            data['S2'] = S2

    if args['--check']:

        import h5py

        with h5py.File('S2_ensemble.h5', 'r') as data:
            assert np.allclose(data['S2'][...], S2)

    if args['--plot']:

        import matplotlib.pyplot as plt
        import matplotlib as mpl
        import matplotlib.cm as cm
        from mpl_toolkits.axes_grid1 import make_axes_locatable

        try:
            plt.style.use(['goose', 'goose-latex'])
        except:
            pass

        fig, ax = plt.subplots()

        phi = S2[50, 50]
        im = ax.imshow(S2, clim=(0, phi), cmap='jet', extent=(-50, 50, -50, 50))
        ax.xaxis.set_ticks([-50, 0, +50])
        ax.yaxis.set_ticks([-50, 0, +50])
        ax.set_xlabel(r'$\Delta x$')
        ax.set_ylabel(r'$\Delta y$')
        ax.set_title (r'$S_2$')
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([0 , phi])
        cbar.set_ticklabels(['0', r'$\varphi$'])

        plt.savefig('S2_ensemble.svg')
