import GooseEYE
import numpy as np

modes = {
    "Bresenham": GooseEYE.path_mode.Bresenham,
    "actual": GooseEYE.path_mode.actual,
    "full": GooseEYE.path_mode.full,
}

images = {}

for mode in modes:
    # calculate a few pixel paths
    paths = (
        GooseEYE.path([0, 0], [9, 2], mode=modes[mode]),
        GooseEYE.path([0, 0], [-3, 9], mode=modes[mode]),
        GooseEYE.path([0, 0], [-8, 9], mode=modes[mode]),
        GooseEYE.path([0, 0], [-9, 0], mode=modes[mode]),
        GooseEYE.path([0, 0], [-9, -3], mode=modes[mode]),
        GooseEYE.path([0, 0], [-2, -9], mode=modes[mode]),
        GooseEYE.path([0, 0], [+9, -2], mode=modes[mode]),
    )

    # plot the paths
    img = np.zeros((19, 19), dtype="int")
    for i, path in enumerate(paths):
        index = np.ravel_multi_index(9 + path.T, img.shape)
        img.flat[index] = (i + 1) * np.ones(path.shape[0])

    images[mode] = img

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

        with h5py.File(root / "pixel_path.h5", "w") as file:
            for mode in modes:
                file[f"/{mode:s}/img"] = images[mode]

    if args.check:
        import h5py

        with h5py.File(root / "pixel_path.h5") as file:
            for mode in modes:
                assert np.all(np.equal(file[mode]["img"][...], images[mode]))

    if args.plot:
        import matplotlib.pyplot as plt

        try:
            plt.style.use(["goose", "goose-latex"])
        except OSError:
            pass

        # open new figure
        fig, axes = plt.subplots(figsize=(18, 6), nrows=1, ncols=3)

        # create a grid of points, to plot a grid (below)
        grid = (
            np.hstack((np.zeros((20, 1)), np.ones((20, 1)))).T,
            np.hstack((np.arange(20).reshape(-1, 1) / 19.0, np.arange(20).reshape(-1, 1) / 19.0)).T,
        )

        # loop over the different algorithms
        for ax, mode in zip(axes, modes):
            ax.imshow(img, cmap="afmhot_r", extent=(0, 0.9999, 0, 0.9999))
            ax.plot(grid[0], grid[1], lw=1, c="k")
            ax.plot(grid[1], grid[0], lw=1, c="k")
            ax.xaxis.set_ticks([])
            ax.yaxis.set_ticks([])
            ax.set_xlabel(r"$\Delta x$")
            ax.set_ylabel(r"$\Delta y$")
            ax.set_title(rf"``{mode:s}''")
            ax.axis("off")

        fig.savefig(root / "pixel_path.svg")
        plt.close(fig)
