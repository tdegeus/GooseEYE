import numpy as np
import GooseEYE

data = np.random.random((40, 40, 40))

ensemble = GooseEYE.Ensemble([1], True, True)

for i in range(data.shape[0]):
    ensemble.mean(data[i,:,:])

print(ensemble.result(), np.mean(data))
print(ensemble.variance(), np.var(data))

