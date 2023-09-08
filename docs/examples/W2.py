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

# image + "damage" + correlation
# ------------------------------

# square grid of circles
N = 15
M = 500
row = np.linspace(0, M, N)
col = np.linspace(0, M, N)
row, col = np.meshgrid(row, col, indexing="ij")  # ('indexing' only for comparison with C++ code)
row = row.reshape(-1)
col = col.reshape(-1)
r = float(M) / float(N) / 4.0 * np.ones(N * N)

# random perturbation
row += GooseEYE.random.normal([N * N], 0.0, float(M) / float(N))
col += GooseEYE.random.normal([N * N], 0.0, float(M) / float(N))
r *= GooseEYE.random.random([N * N]) * 2.0 + 0.1

# generate image, extract 'volume-fraction' for plotting
img = GooseEYE.dummy_circles((M, M), np.round(row), np.round(col), np.round(r))
phi = np.mean(img)

# create 'damage' -> right of inclusion
col += 1.1 * r
r *= 0.4
W = GooseEYE.dummy_circles((M, M), np.round(row), np.round(col), np.round(r))
W[img == 1] = 0

# weighted correlation
WI = GooseEYE.W2((101, 101), W, img, fmask=W)

# gray-scale image + correlation
# ------------------------------

# convert to gray-scale image and introduce noise
Igr = np.array(img, copy=True).astype(float)
Igr += 0.1 * (2.0 * GooseEYE.random.random(Igr.shape) - 1.0) + 0.1
Igr /= 1.2

# mean intensity (for bounds)
Iav = np.mean(Igr)

# weighted correlation
WIgr = GooseEYE.W2((101, 101), W.astype(float), Igr, fmask=W)
# </snippet>

if __name__ == "__main__":
    import docopt

    args = docopt.docopt(__doc__)

    if args["--save"]:
        import h5py

        with h5py.File("W2.h5", "w") as data:
            data["I"] = img
            data["Igr"] = Igr
            data["W"] = W
            data["WI"] = WI
            data["WIgr"] = WIgr

    if args["--check"]:
        import h5py

        with h5py.File("W2.h5", "r") as data:
            assert np.all(np.equal(data["I"][...], img))
            assert np.all(np.equal(data["W"][...], W))
            assert np.allclose(data["Igr"][...], Igr)
            assert np.allclose(data["WI"][...], WI)
            assert np.allclose(data["WIgr"][...], WIgr)

    if args["--plot"]:
        import matplotlib.pyplot as plt
        import matplotlib as mpl
        import matplotlib.cm as cm
        from mpl_toolkits.axes_grid1 import make_axes_locatable

        try:
            plt.style.use(["goose", "goose-latex"])
        except OSError:
            pass

        cmap = mpl.colors.ListedColormap(
            np.array([[0.0, 0.0, 0.0, 0.0], [1.0, 0.0, 0.0, 1.0]], dtype="float64")
        )

        fig, axes = plt.subplots(figsize=(18, 12), nrows=2, ncols=3)

        ax = axes[0, 0]
        im = ax.imshow(img, clim=(0, 1), cmap=mpl.colors.ListedColormap(cm.gray([0, 255])))
        D = ax.imshow(W, clim=(0, 1), cmap=cmap)
        ax.xaxis.set_ticks([0, 500])
        ax.yaxis.set_ticks([0, 500])
        ax.set_xlabel(r"$x$")
        ax.set_ylabel(r"$y$")
        ax.set_title(r"$\mathcal{I}$ (black/white) + $\mathcal{W}$ (red)")
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([0, 1])

        ax = axes[0, 1]
        im = ax.imshow(WI, clim=(0, 1), cmap="jet", extent=(-50, 50, -50, 50))
        ax.xaxis.set_ticks([-50, 0, +50])
        ax.yaxis.set_ticks([-50, 0, +50])
        ax.set_xlabel(r"$\Delta x$")
        ax.set_ylabel(r"$\Delta y$")
        ax.set_title(r"$W_2$")
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([0, 1])
        cbar.set_ticklabels(["0", "1"])

        ax = axes[0, 2]
        im = ax.imshow(WI - phi, clim=(-phi, +phi), cmap="RdBu_r", extent=(-50, 50, -50, 50))
        ax.xaxis.set_ticks([-50, 0, +50])
        ax.yaxis.set_ticks([-50, 0, +50])
        ax.set_xlabel(r"$\Delta x$")
        ax.set_ylabel(r"$\Delta y$")
        ax.set_title(r"$W_2 - \varphi$")
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([-phi, 0, +phi])
        cbar.set_ticklabels([r"$-\varphi$", "0", r"$+\varphi$"])

        ax = axes[1, 0]
        im = ax.imshow(Igr, clim=(0, 1), cmap="gray")
        D = ax.imshow(W, clim=(0, 1), cmap=cmap)
        ax.xaxis.set_ticks([0, 500])
        ax.yaxis.set_ticks([0, 500])
        ax.set_xlabel(r"$x$")
        ax.set_ylabel(r"$y$")
        ax.set_title(r"$\mathcal{I}$ (gray) + $\mathcal{W}$ (red)")
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([0, 1])

        ax = axes[1, 1]
        im = ax.imshow(WIgr, clim=(0, 1), cmap="jet", extent=(-50, 50, -50, 50))
        ax.xaxis.set_ticks([-50, 0, +50])
        ax.yaxis.set_ticks([-50, 0, +50])
        ax.set_xlabel(r"$\Delta x$")
        ax.set_ylabel(r"$\Delta y$")
        ax.set_title(r"$W_2$")
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([0, 1])
        cbar.set_ticklabels(["0", "1"])

        ax = axes[1, 2]
        im = ax.imshow(WIgr - Iav, clim=(-Iav, +Iav), cmap="RdBu_r", extent=(-50, 50, -50, 50))
        ax.xaxis.set_ticks([-50, 0, +50])
        ax.yaxis.set_ticks([-50, 0, +50])
        ax.set_xlabel(r"$\Delta x$")
        ax.set_ylabel(r"$\Delta y$")
        ax.set_title(r"$W_2 - \langle \mathcal{I} \rangle$")
        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([-Iav, 0, +Iav])
        cbar.set_ticklabels(
            [r"$-\langle \mathcal{I} \rangle$", "0", r"$+\langle \mathcal{I} \rangle$"]
        )

        plt.savefig("W2.svg")
