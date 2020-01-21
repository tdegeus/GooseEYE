# <snippet>
import numpy as np
import GooseEYE

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

# skip plot with "--no-plot" command line argument
# ------------------------------------------------

import sys

if len(sys.argv) == 2:
    if sys.argv[1] == "--no-plot":
        sys.exit(0)

# plot
# ----

import matplotlib.pyplot as plt

try:
    plt.style.use(['goose', 'goose-latex'])

except:
    pass

fig, axes = plt.subplots(ncols=2, figsize=(2 * 8, 6))

ax = axes[0]
ax.plot(x, y1, label=r'$\sin (x)$')
ax.plot(x, y2, label=r'$\sin (2x)$', ls='--')
ax.set_xlabel(r'$x$')
ax.set_ylabel(r'$z$')
ax.legend()

ax = axes[1]
ax.plot(dx, hh1, label=r'$\sin (x)$')
ax.plot(dx, hh2, label=r'$\sin (2x)$', ls='--')
ax.set_xlabel(r'$\Delta x$')
ax.set_ylabel(r'$|| z(x) - z(x + \Delta x) ||$')
ax.legend()

plt.savefig('heightheight.svg')
plt.close()
