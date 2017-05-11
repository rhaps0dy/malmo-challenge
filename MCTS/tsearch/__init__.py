from ctypes import *
import numpy as np
from numpy.ctypeslib import as_ctypes
import os

__all__ = ['best_action', 'D_NORTH', 'D_EAST', 'D_SOUTH', 'D_WEST']

D_NORTH = 0
D_EAST = 1
D_SOUTH = 2
D_WEST = 3

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

ACTIONS = ["turn -1", "turn 1", "move 1"]

def best_action(x0, y0, d0, x1, y1, d1, P_x, P_y, budget, c, strat_probs):
    c = switchable_c_float(c)
    strat_probs = as_ctypes(strat_probs.astype(switchable_np_float))
    a = uct.ffi_best_action(x0, y0, d0, x1, y1, d1, P_x, P_y, budget, c,
                            strat_probs, len(strat_probs))
    return ACTIONS[a]
