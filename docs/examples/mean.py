import GooseEYE
import numpy as np
import prrng

rng = prrng.pcg32(0)
random_data = rng.random((40, 40, 40))

ensemble = GooseEYE.Ensemble([1], True, True)

for i in range(random_data.shape[0]):
    ensemble.mean(random_data[i, :, :])

assert np.isclose(ensemble.result()[0], np.mean(random_data))
assert np.isclose(ensemble.variance()[0], np.var(random_data), rtol=1e-4)

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

        with h5py.File(root / "mean.h5", "w") as file:
            file["mean"] = ensemble.result()
            file["variance"] = ensemble.variance()

    if args.check:
        import h5py

        with h5py.File(root / "mean.h5") as file:
            assert np.isclose(file["mean"][...], ensemble.result())
            assert np.isclose(file["variance"][...], ensemble.variance())

    if args.plot:
        pass
