
import numpy          as np
import gooseeye.image as gimage

# # print(gimage.S2(np.arange(10).reshape(5,2),periodic=True))


f   = np.arange(2*3).reshape(2,3)
roi = gimage.S2(f,f)
print(roi)

f   = np.arange(2*3).reshape(2,3).astype(np.float)
roi = gimage.S2(f,f)
print(roi)

