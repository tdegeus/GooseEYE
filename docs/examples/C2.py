# <snippet>
import GooseEYE
import numpy as np

# generate image, extract 'volume-fraction' for plotting
img = GooseEYE.dummy_circles((500, 500))
phi = np.mean(img)

# 2-point probability for comparison
S2 = GooseEYE.S2((101, 101), img, img)

# determine clusters, based on the binary image
C = GooseEYE.clusters(img)

# 2-point cluster function
C2 = GooseEYE.C2((101, 101), C, C)
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

        with h5py.File(root / "C2.h5", "w") as file:
            file["I"] = img
            file["C"] = C
            file["C2"] = C2

    if args.check:
        import h5py

        with h5py.File(root / "C2.h5") as file:
            assert np.all(np.equal(file["I"][...], img))
            assert np.all(np.equal(file["C"][...], C))
            assert np.allclose(file["C2"][...], C2)

    if args.plot:
        import matplotlib.pyplot as plt
        import matplotlib as mpl
        import matplotlib.cm as cm
        from mpl_toolkits.axes_grid1 import make_axes_locatable

        try:
            plt.style.use(["goose", "goose-latex"])
        except OSError:
            pass

        # color-scheme: modify such that the background is white
        cmap = cm.jet(range(256))
        cmap[0, :3] = 1.0  # N.B. for a transparent background -> 4th column == 1.0
        cmap = mpl.colors.ListedColormap(cmap)

        fig, axes = plt.subplots(figsize=(18, 12), nrows=2, ncols=3)

        ax = axes[0, 0]
        im = ax.imshow(img, clim=(0, 1), cmap=mpl.colors.ListedColormap(cm.gray([0, 255])))
        ax.xaxis.set_ticks([0, 500])
        ax.yaxis.set_ticks([0, 500])
        ax.set_xlabel(r"$x$")
        ax.set_ylabel(r"$y$")
        ax.set_title(r"$\mathcal{I}$")
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([0, 1])

        ax = axes[0, 1]
        im = ax.imshow(S2, clim=(0, phi), cmap="jet", extent=(-50, 50, -50, 50))
        ax.xaxis.set_ticks([-50, 0, +50])
        ax.yaxis.set_ticks([-50, 0, +50])
        ax.set_xlabel(r"$\Delta x$")
        ax.set_ylabel(r"$\Delta y$")
        ax.set_title(r"$S_2$")
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([0, phi])
        cbar.set_ticklabels(["0", r"$\varphi$"])

        ax = axes[0, 2]
        ax.plot([-50, 50], [phi**2.0, phi**2.0], color="k", linestyle="--", linewidth=1)
        ax.plot([-50, 50], [phi, phi], color="k", linestyle="--", linewidth=1)
        ax.plot(np.arange(-50, 51), S2[50, :], color="b")
        ax.xaxis.set_ticks([-50, 0, +50])
        ax.yaxis.set_ticks([0, phi**2.0, phi])
        ax.yaxis.set_ticklabels([0, r"$\varphi^2$", r"$\varphi$"])
        ax.set_xlim([-50, +50])
        ax.set_ylim([0, phi])
        ax.set_xlabel(r"$\Delta x$")
        ax.set_ylabel(r"$S_2$")

        ax = axes[1, 0]
        im = ax.imshow(C, clim=(0, np.max(C) + 1), cmap=cmap)
        ax.xaxis.set_ticks([0, 500])
        ax.yaxis.set_ticks([0, 500])
        ax.set_xlim([0, 500])
        ax.set_ylim([0, 500])
        ax.set_xlabel(r"$x$")
        ax.set_ylabel(r"$y$")
        ax.set_title(r"clusters")
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([])

        ax = axes[1, 1]
        im = ax.imshow(C2, clim=(0, phi), cmap="jet", extent=(-50, 50, -50, 50))
        ax.xaxis.set_ticks([-50, 0, +50])
        ax.yaxis.set_ticks([-50, 0, +50])
        ax.set_xlabel(r"$\Delta x$")
        ax.set_ylabel(r"$\Delta y$")
        ax.set_title(r"$C_2$")
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([0, phi])
        cbar.set_ticklabels(["0", r"$\varphi$"])

        ax = axes[1, 2]
        ax.plot(np.arange(-50, 51), C2[50, :], color="b")
        ax.xaxis.set_ticks([-50, 0, +50])
        ax.yaxis.set_ticks([0, phi])
        ax.yaxis.set_ticklabels([0, r"$\varphi$"])
        ax.set_xlim([-50, +50])
        ax.set_ylim([0, phi])
        ax.set_xlabel(r"$\Delta x$")
        ax.set_ylabel(r"$C_2$")

        fig.savefig(root / "C2.svg")
        plt.close(fig)
