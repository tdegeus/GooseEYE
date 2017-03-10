
import numpy          as np
import gooseeye.image as gimage


f = gimage.dummy_circles([50,50],[0,10],[0,10],[5,5])

S2,N = gimage.S2(f,f,[11,11])

print(S2)

S2,N = gimage.S2(f.astype(np.float),f.astype(np.float),[11,11])

print(S2)

# # # print(gimage.S2(np.arange(10).reshape(5,20),periodic=True))


# f   = np.arange(20*30).reshape(20,30)
# roi = gimage.S2(f,f)
# print(roi)

# # f   = np.arange(20*30).reshape(20,30).astype(np.float)
# # roi = gimage.S2(f,f)
# # print(roi)

