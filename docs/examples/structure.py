# <snippet>
import GooseEYE
import numpy as np
import prrng

rng = prrng.pcg32(0)
structure = GooseEYE.Structure()

for _ in range(2000):
    u = np.hstack((0, np.cumsum(rng.normal([2001]))))  # random walk
    u = u - u * np.arange(u.size) / u.size  # periodic random walk
    structure += u
# </snippet>

if __name__ == "__main__":
    import argparse
    import pathlib

    root = pathlib.Path(__file__).parent

    parser = argparse.ArgumentParser()
    parser.add_argument("--save", action="store_true")
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--plot", action="store_true")
    parser.add_argument("--dark", action="store_true", help="use dark theme to plot")
    parser.add_argument("--show", action="store_true", help="show plot instead of saving it")
    args = parser.parse_args()

    if args.save:
        import h5py

        with h5py.File(root / "structure.h5", "w") as file:
            for key, value in structure:
                file[key] = value

    if args.check:
        import h5py

        with h5py.File(root / "structure.h5") as file:
            for key, value in structure:
                np.allclose(file[key], value)

    if args.plot:
        import matplotlib.pyplot as plt

        try:
            styles = ["goose", "goose-latex"]
            if args.dark:
                styles += ["dark_background"]
            plt.style.use(styles)
        except OSError:
            pass

        fig, ax = plt.subplots()

        ax.set_xscale("log")
        ax.set_yscale("log")

        ax.set_xlabel(r"$2 \sin(|q| / 2)$")
        ax.set_ylabel(r"$S(|q|)$")

        q = structure.qnorm
        s = structure.mean()[1 : q.size]
        q = q[1:]
        q *= 2 * np.pi
        q = 2 * np.sin(q * 0.5)
        scaling = 1 / (q**2)
        scaling *= s[1] / scaling[1]

        ax.plot(q, s, marker=".")
        ax.plot(q, scaling, ls="--")

        if args.show:
            plt.show()
        else:
            if args.dark:
                plt.savefig(root / "structure-dark.svg")
            else:
                fig.savefig(root / "structure.svg")

        plt.close(fig)
