from __future__ import division, print_function

from ctypes import *
import numpy as np
from numpy.ctypeslib import as_ctypes
import os
import sys

__all__ = ['BayesianPlanner', 'focused', 'random', 'D_NORTH', 'D_EAST',
           'D_SOUTH', 'D_WEST', 'A_FRONT', 'A_LEFT', 'A_RIGHT']

D_NORTH = 0
D_EAST  = 1
D_SOUTH = 2
D_WEST  = 3

A_FRONT = 0
A_LEFT  = 1
A_RIGHT = 2

if sys.platform.startswith('win') or sys.platform.startswith('cygwin'):
    library_extension = 'dll'
else:
    library_extension = 'so'
library_name = 'libuct.{:s}'.format(library_extension)
uct = cdll.LoadLibrary(os.path.join(os.path.dirname(__file__), library_name))

Float_size = 'float'
if Float_size == 'float':
    switchable_np_float = np.float32
    switchable_c_float = c_float
elif Float_size == 'double':
    switchable_np_float = np.float64
    switchable_c_float = c_double

uct.ffi_best_action.argtypes = [c_int, switchable_c_float,
                                c_void_p, c_size_t,
                                c_int, c_int,
                                c_int, c_int, c_int,
                                c_int, c_int, c_int]
uct.ffi_best_action.restype = c_int

uct.ffi_print_state.argtypes = [c_int, c_int,
                                c_int, c_int, c_int,
                                c_int, c_int, c_int]
uct.ffi_print_state.restype = None


PIG_OPTIMAL_COSTS_SHAPE = (7, 9, 7, 9, 4)
uct.ffi_pig_optimal_costs.argtypes = []
uct.ffi_pig_optimal_costs.restype = np.ctypeslib.ndpointer(
    dtype=np.uint8, shape=PIG_OPTIMAL_COSTS_SHAPE, flags='aligned, contiguous')

class BayesianPlanner:
    def __init__(self):
        self._focused_costs = uct.ffi_pig_optimal_costs()
        self._strats = np.zeros([2], dtype=switchable_np_float)

    def reset(self, strategy_proba):
        self._strats[:] = strategy_proba

    def infer_strategy_proba(self, prev_state, cur_state):
        # Focused
        print("prev_state", prev_state[:5], self._focused_costs[tuple(prev_state[:5])])
        print("cur_state", cur_state[:5], self._focused_costs[tuple(prev_state[:2] + cur_state[2:5])])
        self.print_state(prev_state)
        self.print_state(cur_state)
        try:
            if (self._focused_costs[tuple(prev_state[:2] + cur_state[2:5])] <
                self._focused_costs[tuple(prev_state[:5])]):
                self._strats[0] *= 0.98
            else:
                self._strats[0] *= 0.01
        except ValueError:
            import pdb
            pdb.set_trace()
        # Random
        self._strats[1] *= 1/3
        self._strats /= self._strats[0]+self._strats[1]

    def plan_best_action(self, cur_state, budget=1000, exploration_constant=2.0):
        return uct.ffi_best_action(budget, exploration_constant,
                                   as_ctypes(self._strats), len(self._strats),
                                   *cur_state)

    def print_state(self, state):
        uct.ffi_print_state(*state)


if __name__ == '__main__':
    prev_state = [5, 2, 5, 4, D_WEST, 2, 2, D_SOUTH]
    cur_state = [5, 2, 5, 3, D_WEST, 2, 2, D_SOUTH]
    cur_state2 = [5, 2, 5, 4, D_SOUTH, 2, 2, D_SOUTH]
    bp = BayesianPlanner()
    bp.reset([0.75, 0.25])
    bp.infer_strategy_proba(prev_state, cur_state)
    print(bp._strats)
    bp.infer_strategy_proba(prev_state, cur_state2)
    print(bp._strats)

    bp.print_state(prev_state)
    bp.print_state(cur_state)
    bp.print_state(cur_state2)
    print(bp.plan_best_action(cur_state, budget=100000))
    print('===')
