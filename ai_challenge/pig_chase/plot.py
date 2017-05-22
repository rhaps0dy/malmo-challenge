from __future__ import division

import pickle
import matplotlib.pyplot as plt
import sys

with open(sys.argv[1], 'rb') as f:
     a = pickle.load(f)
a1, a2 = zip(*a[2])

b1 = []
b2 = []
for i in range(1, len(a1)):
    b1.append(a1[i]-a1[i-1])
    b2.append(a2[i]-a2[i-1])
#plt.scatter(a1, a2)
#plt.show()
plt.scatter(b1, b2)
plt.show()






