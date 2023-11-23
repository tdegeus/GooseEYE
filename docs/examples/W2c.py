# <snippet>
import GooseEYE
import numpy as np
import prrng

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
row += rng.normal([N * N], 0.0, float(M) / float(N))
col += rng.normal([N * N], 0.0, float(M) / float(N))
r *= rng.random([N * N]) * 2.0 + 0.1

# generate image, extract 'volume-fraction' for plotting
img = GooseEYE.dummy_circles((M, M), np.round(row), np.round(col), np.round(r))
phi = np.mean(img)

# create 'damage' -> right of inclusion
col += 1.1 * r
r *= 0.4
W = GooseEYE.dummy_circles((M, M), np.round(row), np.round(col), np.round(r))
W[img == 1] = 0

# compute individual damage clusters and their centers
Clusters = GooseEYE.Clusters(W)
clusters = Clusters.labels()
centers = Clusters.centers()

# weighted correlation
WI = GooseEYE.W2((101, 101), W, img, fmask=W)

# collapsed weighted correlation
WIc = GooseEYE.W2c((101, 101), clusters, centers, img, fmask=W)
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

        with h5py.File(root / "W2c.h5", "w") as file:
            file["I"] = img
            file["clusters"] = clusters
            file["centers"] = centers
            file["W"] = W
            file["WI"] = WI
            file["WIc"] = WIc

    if args.check:
        import h5py

        with h5py.File(root / "W2c.h5") as file:
            assert np.all(np.equal(file["I"][...], img))
            assert np.all(np.equal(file["clusters"][...], clusters))
            assert np.all(np.equal(file["centers"][...], centers))
            assert np.all(np.equal(file["W"][...], W))
            assert np.allclose(file["WI"][...], WI)
            assert np.allclose(file["WIc"][...], WIc)

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

        fig, axes = plt.subplots(figsize=(18, 6), nrows=1, ncols=3)

        # ---

        ax = axes[0]

        im = ax.imshow(img, clim=(0, 1), cmap=mpl.colors.ListedColormap(cm.gray([0, 255])))

        ax.imshow(W, clim=(0, 1), cmap=cmap)

        ax.xaxis.set_ticks([0, 500])
        ax.yaxis.set_ticks([0, 500])

        ax.set_xlabel(r"$x$")
        ax.set_ylabel(r"$y$")
        ax.set_title(r"$\mathcal{I}$ (black/white) + $\mathcal{W}$ (red)")

        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_ticks([0, 1])

        # ---

        ax = axes[1]

        im = ax.imshow(WI - phi, clim=(-phi, +phi), cmap="RdBu_r", extent=(-50, 50, -50, 50))

        ax.xaxis.set_ticks([-50, 0, +50])
        ax.yaxis.set_ticks([-50, 0, +50])

        ax.set_xlabel(r"$\Delta x$")
        ax.set_ylabel(r"$\Delta y$")
        ax.set_title(r"weighted correlation")

        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)

        cbar.set_ticks([-phi, 0, +phi])
        cbar.set_ticklabels([r"$-\varphi$", "0", r"$+\varphi$"])

        # ---

        ax = axes[2]

        im = ax.imshow(WIc - phi, clim=(-phi, +phi), cmap="RdBu_r", extent=(-50, 50, -50, 50))

        ax.xaxis.set_ticks([-50, 0, +50])
        ax.yaxis.set_ticks([-50, 0, +50])

        ax.set_xlabel(r"$\Delta x$")
        ax.set_ylabel(r"$\Delta y$")
        ax.set_title(r"collapse weights to center")

        div = make_axes_locatable(ax)
        cax = div.append_axes("right", size="5%", pad=0.1)
        cbar = plt.colorbar(im, cax=cax)

        cbar.set_ticks([-phi, 0, +phi])
        cbar.set_ticklabels([r"$-\varphi$", "0", r"$+\varphi$"])

        fig.savefig(root / "W2c.svg")
        plt.close(fig)
