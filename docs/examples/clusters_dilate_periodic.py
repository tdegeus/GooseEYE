# <snippet>
import GooseEYE
import numpy as np

# generate image
img = np.zeros((21, 21), dtype="bool")
img[4, 4] = True
img[18, 19] = True
img[19, 19] = True
img[20, 19] = True
img[19, 18] = True
img[19, 20] = True

# clusters
C = GooseEYE.Clusters(img).labels()

# dilate
CD = GooseEYE.dilate(C)
# </snippet>

if __name__ == "__main__":
    import argparse
    import pathlib

    root = pathlib.Path(__file__).parent

    parser = argparse.ArgumentParser()
    parser.add_argument("--save", action="store_true")
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--plot", action="store_true")
    args = parser.parse_args()

    if args.save:
        import h5py

        with h5py.File(root / "clusters_dilate_periodic.h5", "w") as file:
            file["I"] = img
            file["C"] = C
            file["CD"] = CD

    if args.check:
        import h5py

        with h5py.File(root / "clusters_dilate_periodic.h5") as file:
            assert np.all(np.equal(file["I"][...], img))
            assert np.all(np.equal(file["C"][...], C))
            assert np.all(np.equal(file["CD"][...], CD))

    if args.plot:
        import matplotlib.pyplot as plt
        import matplotlib as mpl
        import matplotlib.cm as cm
        from mpl_toolkits.axes_grid1 import make_axes_locatable

        # color-scheme: modify such that the background is white
        # N.B. for a transparent background -> 4th column == 1.
        cmap = cm.jet(range(256))
        cmap[0, :3] = 1.0
        cmap = mpl.colors.ListedColormap(cmap)

        try:
            plt.style.use(["goose", "goose-latex"])
        except OSError:
            pass

        fig, axes = plt.subplots(figsize=(18, 6), nrows=1, ncols=3)

        ax = axes[0]
        im = ax.imshow(img, clim=(0, 1), cmap=mpl.colors.ListedColormap(cm.gray([0, 255])))
        ax.xaxis.set_ticks([0, 20])
        ax.yaxis.set_ticks([0, 20])
        ax.set_xlim([-0.5, 20.5])
        ax.set_ylim([-0.5, 20.5])
        ax.set_xlabel(r"$x$")
        ax.set_ylabel(r"$y$")
        ax.set_title(r"image")
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([0, 1])

        ax = axes[1]
        im = ax.imshow(CD, clim=(0, np.max(C) + 1), cmap=cmap)
        ax.xaxis.set_ticks([0, 20])
        ax.yaxis.set_ticks([0, 20])
        ax.set_xlim([-0.5, 20.5])
        ax.set_ylim([-0.5, 20.5])
        ax.set_xlabel(r"$x$")
        ax.set_ylabel(r"$y$")
        ax.set_title(r"clusters + dilate")

        ax = axes[2]
        im = ax.imshow(np.tile(CD, (3, 3)), clim=(0, np.max(C) + 1), cmap=cmap)
        ax.xaxis.set_ticks([0, 60])
        ax.yaxis.set_ticks([0, 60])
        ax.set_xlim([-0.5, 60.5])
        ax.set_ylim([-0.5, 60.5])
        ax.set_xlabel(r"$x$")
        ax.set_ylabel(r"$y$")
        ax.set_title(r"periodic copy")

        fig.savefig(root / "clusters_dilate_periodic.svg")
        plt.close(fig)
