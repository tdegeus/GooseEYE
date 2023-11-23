# <snippet>
import GooseEYE
import numpy as np

ensemble = GooseEYE.Ensemble((101, 101))

for i in range(5):
    img = GooseEYE.dummy_circles((200, 200))
    ensemble.S2(img, img)

S2 = ensemble.result()
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

        with h5py.File(root / "S2_ensemble.h5", "w") as file:
            file["S2"] = S2

    if args.check:
        import h5py

        with h5py.File(root / "S2_ensemble.h5") as file:
            assert np.allclose(file["S2"][...], S2)

    if args.plot:
        import matplotlib.pyplot as plt
        from mpl_toolkits.axes_grid1 import make_axes_locatable

        try:
            plt.style.use(["goose", "goose-latex"])
        except OSError:
            pass

        fig, ax = plt.subplots()

        phi = S2[50, 50]
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

        fig.savefig(root / "S2_ensemble.svg")
        plt.close(fig)
