r"""
    Plot and/or check.

Usage:
    script [options]

Options:
    -s, --save      Save output for later check.
    -c, --check     Check against earlier results.
    -p, --plot      Plot.
    -h, --help      Show this help.
"""
# <snippet>
import GooseEYE
import numpy as np

# binary image + correlation
# --------------------------

# generate image, extract 'volume-fraction' for plotting
img = GooseEYE.dummy_circles((500, 500))
phi = np.mean(img)

# 2-point probability
S2 = GooseEYE.S2((101, 101), img, img)

# grey image + correlation
# ------------------------

# noise
noise = 0.1 * (2.0 * GooseEYE.random.random(img.shape) - 1.0)

# grey-scale image
Igr = np.array(img, copy=True).astype(np.float)
Igr = (Igr + 0.1) / 1.2 + noise

# 2-point correlation ('auto-correlation')
S2gr = GooseEYE.S2((101, 101), Igr, Igr)

# correlation bounds: mean intensity squared and mean of the intensity squared
Iav_sq = np.mean(Igr) ** 2.0
Isq_av = np.mean(Igr**2.0)
# </snippet>

if __name__ == "__main__":

    import docopt

    args = docopt.docopt(__doc__)

    if args["--save"]:

        import h5py

        with h5py.File("S2_autocorrelation.h5", "w") as data:
            data["I"] = img
            data["S2"] = S2
            data["Igr"] = Igr
            data["S2gr"] = S2gr

    if args["--check"]:

        import h5py

        with h5py.File("S2_autocorrelation.h5", "r") as data:
            assert np.all(np.equal(data["I"][...], img))
            assert np.allclose(data["S2"][...], S2)
            assert np.allclose(data["Igr"][...], Igr)
            assert np.allclose(data["S2gr"][...], S2gr)

    if args["--plot"]:

        import matplotlib.pyplot as plt
        import matplotlib as mpl
        import matplotlib.cm as cm
        from mpl_toolkits.axes_grid1 import make_axes_locatable

        try:
            plt.style.use(["goose", "goose-latex"])
        except OSError:
            pass

        fig, axes = plt.subplots(figsize=(18, 12), nrows=2, ncols=3)

        ax = axes[0, 0]
        im = ax.imshow(img, clim=(0, 1), cmap=mpl.colors.ListedColormap(cm.gray([0, 255])))
        ax.xaxis.set_ticks([0, 500])
        ax.yaxis.set_ticks([0, 500])
        ax.set_xlabel(r"$x$")
        ax.set_ylabel(r"$y$")
        ax.set_title(r"$\mathcal{I}$ (black/white)")
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
        ax.plot(np.arange(-50, 51), S2[50, :], color="b")
        ax.xaxis.set_ticks([-50, 0, +50])
        ax.yaxis.set_ticks([0, phi**2.0, phi])
        ax.yaxis.set_ticklabels([0, r"$\varphi^2$", r"$\varphi$"])
        ax.set_xlim([-50, +50])
        ax.set_ylim([0, phi])
        ax.set_xlabel(r"$\Delta x$")
        ax.set_ylabel(r"$S_2$")

        ax = axes[1, 0]
        im = ax.imshow(Igr, clim=(0, 1), cmap="gray")
        ax.xaxis.set_ticks([0, 500])
        ax.yaxis.set_ticks([0, 500])
        ax.set_xlabel(r"$x$")
        ax.set_ylabel(r"$y$")
        ax.set_title(r"$\mathcal{I}$ (gray)")
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([0, 1])

        ax = axes[1, 1]
        im = ax.imshow(S2gr, clim=(0, phi), cmap="jet", extent=(-50, 50, -50, 50))
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

        ax = axes[1, 2]
        ax.plot([-50, 50], [Iav_sq, Iav_sq], color="k", linestyle="--", linewidth=1)
        ax.plot([-50, 50], [Isq_av, Isq_av], color="k", linestyle="--", linewidth=1)
        ax.plot(np.arange(-50, 51), S2gr[50, :], color="b")
        ax.xaxis.set_ticks([-50, 0, +50])
        ax.yaxis.set_ticks([0, Iav_sq, Isq_av, phi])
        ax.yaxis.set_ticklabels(
            [
                0,
                r"$\langle \mathcal{I} \rangle^2$",
                r"$\langle \mathcal{I}^2 \rangle$",
                r"$\varphi$",
            ]
        )
        ax.set_xlim([-50, +50])
        ax.set_ylim([0, phi])
        ax.set_xlabel(r"$\Delta x$")
        ax.set_ylabel(r"$S_2$")

        plt.savefig("S2_autocorrelation.svg")
