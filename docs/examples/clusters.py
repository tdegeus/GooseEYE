# <snippet>
import GooseEYE
import numpy as np

# generate image
img = GooseEYE.dummy_circles((500, 500), periodic=True)

# clusters
labels = GooseEYE.clusters(img, periodic=False)

# clusters, if the image is periodic
labels_periodic = GooseEYE.clusters(img, periodic=True)
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
            file["clusters"] = labels
            file["clusters_periodic"] = labels_periodic

    if args.check:
        import h5py

        with h5py.File(root / "clusters.h5") as file:
            assert np.all(np.equal(file["I"][...], img))
            assert np.all(np.equal(file["clusters"][...], labels))
            assert np.all(np.equal(file["clusters_periodic"][...], labels_periodic))

    if args.plot or args.show:
        import matplotlib.pyplot as plt
        import matplotlib as mpl
        import cppcolormap as cm
        import prrng
        from mpl_toolkits.axes_grid1 import make_axes_locatable

        names = np.unique(labels)
        names_periodic = np.unique(labels_periodic)
        cmap = cm.jet(names.size)
        cmap[0, :3] = 1.0

        rng = prrng.pcg32()
        names = names.astype(np.int64)
        index = 1 + np.arange(names.size - 1)
        rng.shuffle(index)
        cmap = cmap[[0] + list(index), :]

        lmap = GooseEYE.labels_map(labels_periodic, labels)
        assert names_periodic.size == lmap.shape[0]
        assert np.unique(lmap[:, 0]).size == lmap.shape[0]
        assert np.unique(lmap[:, 1]).size == lmap.shape[0]
        labels_periodic = GooseEYE.labels_rename(labels_periodic, lmap)

        try:
            plt.style.use(["goose", "goose-latex"])
        except OSError:
            pass

        fig, axes = plt.subplots(figsize=(8 * 3, 6), nrows=1, ncols=4, constrained_layout=True)

        ax = axes[0]
        bw = mpl.colors.ListedColormap(np.array([[1, 1, 1, 1], [0, 0, 0, 1]]))
        im = ax.imshow(img, clim=(0, 1), cmap=bw)
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
        im = ax.imshow(labels, clim=(0, names.size), cmap=mpl.colors.ListedColormap(cmap))
        ax.xaxis.set_ticks([0, 500])
        ax.yaxis.set_ticks([0, 500])
        ax.set_xlim([0, 500])
        ax.set_ylim([0, 500])
        ax.set_xlabel(r"$x$")
        ax.set_ylabel(r"$y$")
        ax.set_title(r"clusters")

        ax = axes[2]
        im = ax.imshow(labels_periodic, clim=(0, names.size), cmap=mpl.colors.ListedColormap(cmap))
        ax.xaxis.set_ticks([0, 500])
        ax.yaxis.set_ticks([0, 500])
        ax.set_xlim([0, 500])
        ax.set_ylim([0, 500])
        ax.set_xlabel(r"$x$")
        ax.set_ylabel(r"$y$")
        ax.set_title(r"clusters (periodic)")

        ax = axes[3]
        im = ax.imshow(
            np.where(labels_periodic != labels, labels_periodic, 0),
            clim=(0, names.size),
            cmap=mpl.colors.ListedColormap(cmap),
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
            fig.savefig(root / "clusters.svg", bbox_inches="tight")
        plt.close(fig)
