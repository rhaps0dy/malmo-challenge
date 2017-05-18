from __future__ import division, print_function

from ctypes import *
import numpy as np
from numpy.ctypeslib import as_ctypes
import os

#class Strategy:
#    n_actions = 3
#    def actionProba(self, state, action, time_step):
#        raise NotImplementedError
#
#class RandomStrategy(Strategy):
#    def actionProba(self, state, action, time_step):
#        return 1/self.n_actions
#
#class ExitStrategy(Strategy):
#    def actionProba(self, state, action, time_step):
#        return [0.1, 0.6, 0.3][action]
#
#class PathPigStrategy(Strategy):
#    def actionProba(self, state, action, time_step):
#        return [0.98, 0.01, 0.01][action]

__all__ = ['BayesianPlanner', 'focused', 'random', 'D_NORTH', 'D_EAST',
           'D_SOUTH', 'D_WEST', 'A_FRONT', 'A_LEFT', 'A_RIGHT']

D_NORTH = 0
D_EAST = 1
D_SOUTH = 2
D_WEST = 3

A_FRONT = 0
A_LEFT = 1
A_RIGHT = 2

uct = cdll.LoadLibrary(os.path.join(os.path.dirname(__file__), 'libuct.so'))

Float_size = 'float'
if Float_size == 'float':
    switchable_np_float = np.float32
    switchable_c_float = c_float
elif Float_size == 'double':
    switchable_np_float = np.float64
    switchable_c_float = c_double

uct.ffi_best_action.argtypes = [c_int, c_int, c_int,
                                c_int, c_int, c_int,
                                c_int, c_int,
                                c_int, switchable_c_float,
                                c_void_p, c_size_t]
uct.ffi_best_action.restype = c_int

PIG_OPTIMAL_COSTS_SHAPE = (5, 5, 5, 5, 4)
uct.ffi_pig_optimal_costs.argtypes = []
uct.ffi_pig_optimal_costs.restype = np.ctypeslib.ndpointer(
    dtype=np.uint8, shape=PIG_OPTIMAL_COSTS_SHAPE, flags='aligned, contiguous')

class BayesianPlanner:
    def __init__(self, _strategies):
        self._focused_costs = uct.pig_optimal_costs()
        self._strats = np.zeros([2], dtype=np.float32)

    def set_strategy_proba(p):
        self._strats[:] = p

    def infer_strategy_proba(prev_state, cur_state):
        if prev_state[7] == cur_state[7]:
            action = A_FRONT
        else:
            if (prev_state[7]+1)%4 == cur_state[7]:
                action = A_RIGHT
            else:
                action = A_LEFT
        # Focused
        if (self._focused_costs[cur_state[:5]] <
            self._focused_costs[prev_state[:5]]):
            self._strats[0] *= 0.98
        else:
            self._strats[0] *= 0.01
        # Random
        self._strats[1] *= 1/3
        self._strats /= self._strats[0]+self._strats[1]

    def plan_best_action(cur_state):
        return A_RIGHT

def best_action(x0, y0, d0, x1, y1, d1, P_x, P_y, budget, c, strat_probs):
    c = switchable_c_float(c)
    strat_probs = as_ctypes(strat_probs.astype(switchable_np_float))
    a = uct.ffi_best_action(x0, y0, d0, x1, y1, d1, P_x, P_y, budget, c,
                            strat_probs, len(strat_probs))
    return a
