import numpy as np
import tsearch as ts

BUDGET=1000000
CONSTANT=30.0

a = ts.best_action(2, 3, ts.D_NORTH, 4, 3, ts.D_EAST, 6, 1, BUDGET, CONSTANT, np.array([0,1]))
assert a == "turn -1", a

a = ts.best_action(2, 3, ts.D_NORTH, 6, 3, ts.D_SOUTH, 5, 3, BUDGET, CONSTANT, np.array([1,0]))
assert a == "turn 1", a

