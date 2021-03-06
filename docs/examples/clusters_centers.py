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

# generate image
I = GooseEYE.dummy_circles((500, 500), periodic=True)

# clusters
clusters = GooseEYE.Clusters(I, periodic=False)
labels = clusters.labels()
centers = clusters.center_positions()

# clusters, if the image is periodic
clusters_periodic = GooseEYE.Clusters(I, periodic=True)
labels_periodic = clusters_periodic.labels()
centers_periodic = clusters_periodic.center_positions()
# </snippet>

if __name__ == '__main__':

    import docopt

    args = docopt.docopt(__doc__)

    if args['--save']:

        import h5py

        with h5py.File('clusters_centers.h5', 'w') as data:
            data['I'] = I
            data['labels'] = labels
            data['centers'] = centers
            data['labels_periodic'] = labels_periodic
            data['centers_periodic'] = centers_periodic

    if args['--check']:

        import h5py

        with h5py.File('clusters_centers.h5', 'r') as data:
            assert np.all(np.equal(data['I'][...], I))
            assert np.all(np.equal(data['labels'][...], labels))
            assert np.allclose(data['centers'][...], centers)
            assert np.all(np.equal(data['labels_periodic'][...], labels_periodic))
            assert np.allclose(data['centers_periodic'][...], centers_periodic)

    if args['--plot']:

        import matplotlib.pyplot as plt
        import matplotlib as mpl
        import matplotlib.cm as cm
        from mpl_toolkits.axes_grid1 import make_axes_locatable

        l = np.sort(np.unique(labels))[1:]
        lp = np.sort(np.unique(labels_periodic))[1:]
        centers = centers[l, :]
        centers_periodic = centers_periodic[lp, :]

        # color-scheme: modify such that the background is white
        # N.B. for a transparent background -> 4th column == 1.
        cmap = cm.jet(range(256))
        cmap[0, :3] = 1.0
        cmap = mpl.colors.ListedColormap(cmap)

        try:
            plt.style.use(['goose', 'goose-latex'])
        except:
            pass

        fig, axes = plt.subplots(figsize=(18, 6), nrows=1, ncols=3)

        ax = axes[0]
        im = ax.imshow(I, clim=(0, 1), cmap=mpl.colors.ListedColormap(cm.gray([0, 255])))
        ax.xaxis.set_ticks([0, 500])
        ax.yaxis.set_ticks([0, 500])
        ax.set_xlim([0, 500])
        ax.set_ylim([0, 500])
        ax.set_xlabel(r'$x$')
        ax.set_ylabel(r'$y$')
        ax.set_title (r'image')
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([0, 1])

        ax = axes[1]
        im = ax.imshow(labels, clim=(0, np.max(labels) + 1), cmap=cmap)
        ax.plot(centers[:, 1], centers[:, 0], ls='none', marker='o', color='r')
        ax.xaxis.set_ticks([0, 500])
        ax.yaxis.set_ticks([0, 500])
        ax.set_xlim([0, 500])
        ax.set_ylim([0, 500])
        ax.set_xlabel(r'$x$')
        ax.set_ylabel(r'$y$')
        ax.set_title (r'clusters')
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([])

        ax = axes[2]
        im = ax.imshow(labels_periodic, clim=(0, np.max(labels) + 1), cmap=cmap)
        ax.plot(centers_periodic[:, 1], centers_periodic[:, 0], ls='none', marker='o', color='r')
        ax.xaxis.set_ticks([0, 500])
        ax.yaxis.set_ticks([0, 500])
        ax.set_xlim([0, 500])
        ax.set_ylim([0, 500])
        ax.set_xlabel(r'$x$')
        ax.set_ylabel(r'$y$')
        ax.set_title (r'clusters (periodic)')
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([])

        plt.savefig('clusters_centers.svg')
