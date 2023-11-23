# <snippet>
import GooseEYE
import numpy as np

# generate image, extract 'volume-fraction' for plotting
img = GooseEYE.dummy_circles((500, 500))
phi = np.mean(img)

# 2-point probability
S2 = GooseEYE.S2((101, 101), img, img)
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

        with h5py.File(root / "S2.h5", "w") as file:
            file["I"] = img
            file["S2"] = S2

    if args.check:
        import h5py

        with h5py.File(root / "S2.h5") as file:
            assert np.all(np.equal(file["I"][...], img))
            assert np.allclose(file["S2"][...], S2)

    if args.plot:
        import matplotlib.pyplot as plt
        import matplotlib as mpl
        import matplotlib.cm as cm
        from mpl_toolkits.axes_grid1 import make_axes_locatable

        try:
            plt.style.use(["goose", "goose-latex"])
        except OSError:
            pass

        fig, axes = plt.subplots(figsize=(18, 6), nrows=1, ncols=3)

        ax = axes[0]
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

        ax = axes[1]
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

        ax = axes[2]
        ax.plot([-50, 50], [phi**2.0, phi**2.0], color="k", linestyle="--", linewidth=1)
        ax.plot(np.arange(-50, 51), S2[50, :], color="b")
        ax.xaxis.set_ticks([-50, 0, +50])
        ax.yaxis.set_ticks([0, phi**2.0, phi])
        ax.yaxis.set_ticklabels([0, r"$\varphi^2$", r"$\varphi$"])
        ax.set_xlim([-50, +50])
        ax.set_ylim([0, phi])
        ax.set_xlabel(r"$\Delta x$")
        ax.set_ylabel(r"$S_2$")

        fig.savefig(root / "S2.svg")
        plt.close(fig)
