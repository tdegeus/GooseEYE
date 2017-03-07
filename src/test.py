
import numpy          as np
import gooseeye.image as gimage

# # print(gimage.S2(np.arange(10).reshape(5,2),periodic=True))

f   = np.arange(2)
roi = gimage.S2(f,f)
print(roi)

f   = np.arange(2*3).reshape(2,3)
roi = gimage.S2(f,f)
print(roi)

f   = np.arange(2*3*4).reshape(2,3,4)
roi = gimage.S2(f,f)
print(roi)


# gimage.test2(np.arange(25).reshape(5,5).astype(np.float64)+.1)

# A = gimage.test3(np.arange(25).reshape(5,5).astype(np.float64)+.1)
# B = gimage.test4(np.arange(3*3*3*3).reshape(3,3,3,3))

# print(A)
# print(A.dtype)


# print(B)
# print(B.dtype)


# print(B*10)

# # print(dir(B))
