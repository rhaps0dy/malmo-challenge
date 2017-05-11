#pragma once

#include <vector>
#include <utility>

#include "common.hpp"
#include "node.hpp"

using namespace std;

typedef Float (*UtilityFun)(Node &);

Float pig_chase_value(Node &n);
Node &simulate_path(Node &current, Float constant);
Action uct_best_action(Node &root, int budget, UtilityFun f, Float constant);

extern "C" {
	Action ffi_best_action(int x0, int y0, Direction d0, int x1, int y1,
						   Direction d1, int P_x, int P_y, int budget, Float c,
						   Float strat_probs[], size_t n_strats);
}
