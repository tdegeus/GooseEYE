
# <snippet>

import GooseEYE as eye
import numpy    as np

x = np.linspace(0, 2.0 * np.pi, 1000)
y1 = np.sin(x)
y2 = np.sin(2. * x)

roughness1 = eye.roughness(roi=[200], f=y1, periodic=True)
roughness2 = eye.roughness(roi=[200], f=y2, periodic=True)
dx = eye.distance(roi=[200], dim=0)

# </snippet>

# plot
# ----

import matplotlib.pyplot as plt

try   : plt.style.use(['goose','goose-latex'])
except: pass

fig, axes = plt.subplots(ncols=2)

ax = axes[0]

ax.plot(x, y1)
ax.plot(x, y2)

ax = axes[1]

ax.plot(dx, roughness1)
ax.plot(dx, roughness2)

plt.show()
