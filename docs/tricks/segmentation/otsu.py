import matplotlib.pyplot as plt
import numpy as np

# ==================================================================================================


def threshold_otsu(image, bins=256):
    r"""
    Return threshold value based on Otsu's method. Code based on
    `Scikit's skimage.filters.threshold_otsu
    <https://github.com/scikit-image/scikit-image/blob/master/skimage/filters/thresholding.py#L230>`_,
    and `Wikipedia <https://en.wikipedia.org/wiki/Otsu's_Method>`_

    :arguments:

      **image** (``<numpy.ndarray>``)
        Two-dimensional greyscale input image.

      **bins** ([``256``] | ``<int>``)
        Number of bins used to calculate histogram. This value is ignored for integer arrays.

    :returns:

      **threshold** (``<float>``)
        Upper threshold value. All pixels with an intensity higher than this value are assumed to be
        foreground.

      **separability** (``<float>``)
        The separability of an image defined as the ratio of the squares of the between-class
        variance and the image's variance.
    """

    assert len(image.shape) == 2
    assert image.min() < image.max()

    # get histogram
    hist, bin_edges = np.histogram(image.ravel(), bins=bins, density=True)
    bin_centers = (bin_edges[:-1] + bin_edges[1:]) / 2.0
    hist = hist.astype(float)

    # class probabilities for all possible thresholds
    weight1 = np.cumsum(hist)
    weight2 = np.cumsum(hist[::-1])[::-1]
    # class means for all possible thresholds
    mean1 = np.cumsum(hist * bin_centers) / weight1
    mean2 = (np.cumsum((hist * bin_centers)[::-1]) / weight2[::-1])[::-1]

    # Clip ends to align class 1 and class 2 variables:
    # The last value of `weight1`/`mean1` should pair with zero values in
    # `weight2`/`mean2`, which do not exist.
    variance12 = weight1[:-1] * weight2[1:] * (mean1[:-1] - mean2[1:]) ** 2

    idx = np.argmax(variance12)
    threshold = bin_centers[:-1][idx]

    separability = variance12[idx] / np.var(image.ravel())

    return (threshold, separability)


# ==================================================================================================

# Dummy image: A

Ia = 0.35 * np.ones((200, 200))
Ia[50:150, 50:150] = 0.65
Ia *= (0.2 * np.random.randn(Ia.size) + 1.0).reshape(Ia.shape)

# Dummy image: B

Ib = 0.45 * np.ones((200, 200))
Ib[50:150, 50:150] = 0.75
Ib *= (0.1 * np.random.randn(Ib.size) + 1.0).reshape(Ib.shape)

# Find optimum weight and construct combined image

alpha = np.linspace(0, 1, 100)

T = np.empty(alpha.shape)
S = np.empty(alpha.shape)

for i, a in enumerate(alpha):
    T[i], S[i] = threshold_otsu(Ia * (1.0 - a) + Ib * a, bins=100)

W = alpha[np.argmax(S)]

img = Ia * (1.0 - W) + Ib

# --------------------------------------------------------------------------------------------------

# Plot
# ----

fig, axes = plt.subplots(ncols=3, nrows=3, figsize=(16, 12))

# Image A
# -------

# image

ax = axes[0, 0]

ax.imshow(Ia, interpolation="nearest", clim=(0, 1), cmap="Greys")

# histogram

P, bin_edges = np.histogram(Ia.ravel(), bins=100, density=True)
x = (bin_edges[:-1] + bin_edges[1:]) / 2.0

threshold, separability = threshold_otsu(Ia, bins=100)

ax = axes[0, 1]

ax.set_xlim([0, 1])
ax.set_xlabel(r"$I$")
ax.set_ylabel(r"$P(I)$")

ax.plot(x, P, color="k")
ax.plot([threshold, threshold], ax.get_ylim(), color="r")

# result

ax = axes[0, 2]

ax.imshow(np.where(Ia < threshold, 0, 1), interpolation="nearest", clim=(0, 1), cmap="Greys")

# Image B
# -------

# image

ax = axes[1, 0]

ax.imshow(Ib, interpolation="nearest", clim=(0, 1), cmap="Greys")

# histogram

P, bin_edges = np.histogram(Ib.ravel(), bins=100, density=True)
x = (bin_edges[:-1] + bin_edges[1:]) / 2.0

threshold, separability = threshold_otsu(Ib, bins=100)

ax = axes[1, 1]

ax.set_xlim([0, 1])
ax.set_xlabel(r"$I$")
ax.set_ylabel(r"$P(I)$")

ax.plot(x, P, color="k")
ax.plot([threshold, threshold], ax.get_ylim(), color="r")

# result

ax = axes[1, 2]

ax.imshow(np.where(Ib < threshold, 0, 1), interpolation="nearest", clim=(0, 1), cmap="Greys")

# Image B
# -------

# separability

ax = axes[2, 0]

ax.set_xlim([0, 1])
ax.set_xlabel(r"$\alpha$")
ax.set_ylabel(r"$S$")

ax.plot(alpha, S, color="k")

ax.plot([W, W], ax.get_ylim(), color="r")

# histogram

P, bin_edges = np.histogram(img.ravel(), bins=100, density=True)
x = (bin_edges[:-1] + bin_edges[1:]) / 2.0

threshold, separability = threshold_otsu(img, bins=100)

ax = axes[2, 1]

ax.set_xlim([0, 1])
ax.set_xlabel(r"$I$")
ax.set_ylabel(r"$P(I)$")

ax.plot(x, P, color="k")
ax.plot([threshold, threshold], ax.get_ylim(), color="r")

# result

ax = axes[2, 2]

ax.imshow(np.where(img < threshold, 0, 1), interpolation="nearest", clim=(0, 1), cmap="Greys")

# Save results
# ------------

plt.savefig("otsu.svg")
plt.show()
