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

structure = GooseEYE.Structure()

for _ in range(2000):
    u = np.hstack((0, np.cumsum(np.random.randn(2001))))  # random walk
    u = u - u * np.arange(u.size) / u.size  # periodic random walk
    structure += u
# </snippet>

if __name__ == "__main__":

    import docopt

    args = docopt.docopt(__doc__)

    if args["--save"]:

        import h5py

        with h5py.File("structure.h5", "w") as data:
            for key, value in structure:
                data[key] = value

    if args["--check"]:

        import h5py

        with h5py.File("structure.h5", "r") as data:
            for key, value in structure:
                np.allclose(data[key], value)

    if args["--plot"]:

        import matplotlib.pyplot as plt

        try:
            plt.style.use(["goose", "goose-latex"])
        except OSError:
            pass

        fig, ax = plt.subplots()

        ax.set_xscale("log")
        ax.set_yscale("log")

        ax.set_xlabel(r"$|q|$")
        ax.set_ylabel(r"$S(|q|)$")

        q = structure.qnorm
        s = structure.mean()[1 : q.size]
        q = q[1:]
        scaling = 1 / (q**2)
        scaling *= s[1] / scaling[1]

        ax.plot(q, s, marker=".")
        ax.plot(q, scaling, ls="--")

        plt.savefig("structure.svg")
