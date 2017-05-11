__all__ = ['best_action']

from ctypes import *
import numpy as np
from numpy.ctypeslib import as_ctypes
import os

uct = cdll.LoadLibrary(os.path.join(os.path.dirname(__file__), 'libmcts.dylib'))

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
    strat_probs = as_ctypes(strat_probs.as_dtype(switchable_np_float))
    a = uct.ffi_best_action(x0, y0, d0, x1, y1, d1, P_x, P_y, budget, c,
                            strat_probs, len(strats_probs))
    return ACTIONS[a]
