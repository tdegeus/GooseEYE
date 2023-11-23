# <snippet>
import GooseEYE
import numpy as np

L = 2.0 * np.pi
N = 1000
h = L / N
x = np.linspace(0, L, N)
y1 = np.sin(x)
y2 = np.sin(2.0 * x)

hh1 = GooseEYE.heightheight(roi=[200], f=y1, periodic=True)
hh2 = GooseEYE.heightheight(roi=[200], f=y2, periodic=True)
dx = GooseEYE.distance(roi=[200], h=[h], dim=0)
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

        with h5py.File(root / "heightheight.h5", "w") as file:
            file["y1"] = y1
            file["y2"] = y2
            file["hh1"] = hh1
            file["hh2"] = hh2
            file["dx"] = dx

    if args.check:
        import h5py

        with h5py.File(root / "heightheight.h5") as file:
            assert np.allclose(file["y1"][...], y1)
            assert np.allclose(file["y2"][...], y2)
            assert np.allclose(file["hh1"][...], hh1)
            assert np.allclose(file["hh2"][...], hh2)
            assert np.allclose(file["dx"][...], dx)

    if args.plot:
        import matplotlib.pyplot as plt

        try:
            plt.style.use(["goose", "goose-latex"])
        except OSError:
            pass

        fig, axes = plt.subplots(ncols=2, figsize=(2 * 8, 6))

        ax = axes[0]
        ax.plot(x, y1, label=r"$\sin (x)$")
        ax.plot(x, y2, label=r"$\sin (2x)$", ls="--")
        ax.set_xlabel(r"$x$")
        ax.set_ylabel(r"$z$")
        ax.legend()

        ax = axes[1]
        ax.plot(dx, hh1, label=r"$\sin (x)$")
        ax.plot(dx, hh2, label=r"$\sin (2x)$", ls="--")
        ax.set_xlabel(r"$\Delta x$")
        ax.set_ylabel(r"$|| z(x) - z(x + \Delta x) ||$")
        ax.legend()

        fig.savefig(root / "heightheight.svg")
        plt.close(fig)
