import matplotlib.pyplot as plt
import numpy as np
import scipy
import scipy.stats
import cPickle as pickle
import collections

with open('beefy.pkl', 'rb') as f:
    d = collections.defaultdict(lambda: [], pickle.load(f))
with open('stone.pkl', 'rb') as f:
    _d = pickle.load(f)
    for k, l in _d.items():
        d[k] += l

pig_l = collections.defaultdict(lambda: [], {})
for k, v in d.iteritems():
    pig_l[tuple(k[:2])] += v

#plt.scatter(*zip(*pig_l.keys()))
#plt.show()

print pig_l[2,3], pig_l[2,5]

def plot(dct, idx):
    xs, ys = zip(*dct[idx])
    plt.scatter(xs, ys)
    plt.show()

out = collections.defaultdict(lambda: [], {})

for (y, x, p1_y, p1_x, p2_y, p2_x), l in d.iteritems():
    if x > 4:
        x = 9-x
        p1_x = 9-p1_x
        p2_x = 9-p2_x
        x_flipped = True
    else:
        x_flipped = False

    if y > 3:
        y = 7-y
        p1_y = 7-p1_y
        p2_y = 7-p2_y
        y_flipped = True
    else:
        y_flipped = False

    if (p1_y, p1_x) > (p2_y, p2_x):
        a = p1_y
        b = p1_x
        p1_y = p2_y
        p1_x = p2_x
        p2_y = a
        p2_x = b

    for pig_dy, pig_dx in l:
        out[y, x, p1_y, p1_x, p2_y, p2_x].append((
            -pig_dy if y_flipped else pig_dy,
            -pig_dx if x_flipped else pig_dx))

for key in out.keys():
    k = tuple(key[:2])

def show_all_configurations():
    points = set()
    for p1_y in range(7):
        for p1_x in range(9):
            for p2_y in range(7):
                for p2_x in range(9):
                    mini_plots = []
                    for pig_y in range(4):
                        for pig_x in range(5):
                            points.add((pig_y, pig_x))
                            t = (pig_y, pig_x, p1_y, p1_x, p2_y, p2_x)
                            for elem in out[t]:
                                mini_plots.append((pig_y+elem[0]*0.1,
                                    pig_x+elem[1]*0.1))
                    if len(mini_plots) == 0:
                        continue
                    plt.scatter(*zip(*mini_plots), c='b')
                    plt.scatter(*zip(*list(points)), c='r')
                    plt.scatter([p1_y, p2_y], [p1_x, p2_x], s=300, c='g')
                    plt.show()

ds = []
for _, ls in out.items():
    for l in ls:
        d = (l[0]**2+l[1]**2)**.5
        if d > 0.01:
            ds.append(d)

np.seterr(all='raise')
#dist_names = [ 'anglit', 'beta', 'bradford', 'chi2', 'expon', 'gamma','halflogistic', 'halfnorm','lognorm', 'pearson3', 'powernorm', 'rayleigh', 'truncexpon', 'uniform']
dist_names = ['halfnorm']
for dist_name in dist_names:
    try:
        dist = getattr(scipy.stats, dist_name)
        params = dist.fit(ds, floc=0)
        print(params)
        x = np.linspace(0, 7, 100)
        plt.plot(x, dist.pdf(x, *params[:-2], loc=params[-2], scale=params[-1])*len(ds), label=dist_name)
    except Exception:
        print('bad', dist_name)
plt.hist(ds)
plt.legend(loc='upper right')
plt.show()
