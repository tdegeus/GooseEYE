# <snippet>
import GooseEYE
import numpy as np
import prrng

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
rng = prrng.pcg32(0)
row += rng.normal(shape=[N * N], mu=0, sigma=M / N)
col += rng.normal(shape=[N * N], mu=0, sigma=M / N)
r *= rng.normal(shape=[N * N], mu=0, sigma=2) + 0.1

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
Igr += 0.1 * (2.0 * rng.random(Igr.shape) - 1.0) + 0.1
Igr /= 1.2

# mean intensity (for bounds)
Iav = np.mean(Igr)

# weighted correlation
WIgr = GooseEYE.W2((101, 101), W.astype(float), Igr, fmask=W)
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

        with h5py.File(root / "W2.h5", "w") as file:
            file["I"] = img
            file["Igr"] = Igr
            file["W"] = W
            file["WI"] = WI
            file["WIgr"] = WIgr

    if args.check:
        import h5py

        with h5py.File(root / "W2.h5") as file:
            assert np.all(np.equal(file["I"][...], img))
            assert np.all(np.equal(file["W"][...], W))
            assert np.allclose(file["Igr"][...], Igr)
            assert np.allclose(file["WI"][...], WI)
            assert np.allclose(file["WIgr"][...], WIgr)

    if args.plot:
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

        fig.savefig(root / "W2.svg")
        plt.close(fig)
