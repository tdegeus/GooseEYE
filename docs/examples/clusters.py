# <snippet>
import GooseEYE
import numpy as np

# generate image
img = GooseEYE.dummy_circles((500, 500), periodic=True)

# clusters
clusters = GooseEYE.clusters(img, periodic=False)

# clusters, if the image is periodic
clusters_periodic = GooseEYE.clusters(img, periodic=True)
# </snippet>

if __name__ == "__main__":
    import argparse
    import pathlib

    root = pathlib.Path(__file__).parent

    parser = argparse.ArgumentParser()
    parser.add_argument("--save", action="store_true")
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--plot", action="store_true")
    parser.add_argument("--show", action="store_true")
    args = parser.parse_args()

    if args.save:
        import h5py

        with h5py.File(root / "clusters.h5", "w") as file:
            file["I"] = img
            file["clusters"] = clusters
            file["clusters_periodic"] = clusters_periodic

    if args.check:
        import h5py

        with h5py.File(root / "clusters.h5") as file:
            assert np.all(np.equal(file["I"][...], img))
            assert np.all(np.equal(file["clusters"][...], clusters))
            assert np.all(np.equal(file["clusters_periodic"][...], clusters_periodic))

    if args.plot or args.show:
        import matplotlib.pyplot as plt
        import matplotlib as mpl
        import matplotlib.cm as cm
        import prrng
        from mpl_toolkits.axes_grid1 import make_axes_locatable

        # color-scheme: modify such that the background is white
        # N.B. for a transparent background -> 4th column == 1.
        cmap = cm.jet(range(256))
        cmap[0, :3] = 1.0
        cmap = mpl.colors.ListedColormap(cmap)

        # reshuffle for better visualisation
        assert np.all(np.diff(np.unique(clusters)) == 1)
        assert np.all(np.diff(np.unique(clusters_periodic)) == 1)
        assert np.unique(clusters).size >= np.unique(clusters_periodic).size
        rng = prrng.pcg32()
        lab = np.unique(clusters)
        lab = lab[lab != 0]
        new = np.copy(lab).astype(np.int64)
        rng.shuffle(new)
        rename = np.array(([0] + list(lab), [0] + list(new))).T
        clusters = GooseEYE.labels_rename(clusters, rename)
        lmap = GooseEYE.labels_map(clusters_periodic, clusters)
        unq, unq_idx, unq_cnt = np.unique(lmap[:, 1], return_inverse=True, return_counts=True)
        assert np.all(np.in1d(np.unique(clusters_periodic), lmap[:, 0]))
        assert np.all(np.equal(np.sort(lmap[:, 0]), np.unique(lmap[:, 0])))
        assert np.all(np.equal(np.sort(lmap[:, 1]), np.unique(lmap[:, 1])))
        clusters_periodic = GooseEYE.labels_rename(clusters_periodic, lmap)

        try:
            plt.style.use(["goose", "goose-latex"])
        except OSError:
            pass

        fig, axes = plt.subplots(figsize=(8 * 4, 6), nrows=1, ncols=4)

        ax = axes[0]
        im = ax.imshow(img, clim=(0, 1), cmap=mpl.colors.ListedColormap(cm.gray([0, 255])))
        ax.xaxis.set_ticks([0, 500])
        ax.yaxis.set_ticks([0, 500])
        ax.set_xlim([0, 500])
        ax.set_ylim([0, 500])
        ax.set_xlabel(r"$x$")
        ax.set_ylabel(r"$y$")
        ax.set_title(r"image")
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([0, 1])

        ax = axes[1]
        im = ax.imshow(clusters, clim=(0, np.max(clusters) + 1), cmap=cmap)
        ax.xaxis.set_ticks([0, 500])
        ax.yaxis.set_ticks([0, 500])
        ax.set_xlim([0, 500])
        ax.set_ylim([0, 500])
        ax.set_xlabel(r"$x$")
        ax.set_ylabel(r"$y$")
        ax.set_title(r"clusters")

        ax = axes[2]
        im = ax.imshow(clusters_periodic, clim=(0, np.max(clusters_periodic) + 1), cmap=cmap)
        ax.xaxis.set_ticks([0, 500])
        ax.yaxis.set_ticks([0, 500])
        ax.set_xlim([0, 500])
        ax.set_ylim([0, 500])
        ax.set_xlabel(r"$x$")
        ax.set_ylabel(r"$y$")
        ax.set_title(r"clusters (periodic)")

        ax = axes[3]
        im = ax.imshow(
            np.where(clusters_periodic != clusters, clusters_periodic, 0),
            clim=(0, np.max(clusters_periodic) + 1),
            cmap=cmap,
        )
        ax.xaxis.set_ticks([0, 500])
        ax.yaxis.set_ticks([0, 500])
        ax.set_xlim([0, 500])
        ax.set_ylim([0, 500])
        ax.set_xlabel(r"$x$")
        ax.set_ylabel(r"$y$")
        ax.set_title(r"difference")

        if args.show:
            plt.show()
        else:
            fig.savefig(root / "clusters.svg")
        plt.close(fig)
