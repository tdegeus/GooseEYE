r'''
    Plot and/or check.

Usage:
    script [options]

Options:
    -s, --save      Save output for later check.
    -c, --check     Check against earlier results.
    -p, --plot      Plot.
    -h, --help      Show this help.
'''

import numpy as np
import GooseEYE

np.random.seed(0)

random_data = GooseEYE.random.random((40, 40, 40))

ensemble = GooseEYE.Ensemble([1], True, True)

for i in range(random_data.shape[0]):
    ensemble.mean(random_data[i,:,:])

assert np.isclose(ensemble.result()[0], np.mean(random_data))
assert np.isclose(ensemble.variance()[0], np.var(random_data), rtol=1e-4)

if __name__ == '__main__':

    import docopt

    args = docopt.docopt(__doc__)

    if args['--save']:

        import h5py

        with h5py.File('mean.h5', 'w') as data:
            data['mean'] = ensemble.result()
            data['variance'] = ensemble.variance()

    if args['--check']:

        import h5py

        with h5py.File('mean.h5', 'r') as data:
            assert np.isclose(data['mean'][...], ensemble.result())
            assert np.isclose(data['variance'][...], ensemble.variance())

    if args['--plot']:

        pass

