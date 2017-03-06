
import numpy          as np
import gooseeye.image as gimage

print(gimage.S2(np.arange(10).reshape(5,2),periodic=True))


gimage.S2_new(np.arange(25).reshape(5,5).astype(np.bool ),np.arange(25).reshape(5,5).astype(np.bool ),[5,5],periodic=True)
gimage.S2_new(np.arange(25).reshape(5,5).astype(np.float),np.arange(25).reshape(5,5).astype(np.float),[5,5],periodic=True)
