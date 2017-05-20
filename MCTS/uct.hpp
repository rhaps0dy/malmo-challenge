#pragma once

#include <vector>
#include <utility>

#include "common.hpp"
#include "node.hpp"

using namespace std;

typedef Float (*RewardFun)(const Node &);

Float pig_chase_reward(const Node &n);
Node &simulate_path(Node &current, Float constant);
Action uct_best_action(Node &root, int budget, RewardFun f, Float constant);

extern "C" {
	Action ffi_best_action(int budget, Float c,
						   Float strat_probs[], size_t n_probs,
						   int P_y, int P_x, int y1, int x1, Direction d1,
						   int y0, int x0, Direction d0);
	void ffi_print_state(int P_y, int P_x, int y1, int x1, Direction d1,
						 int y0, int x0, Direction d0);
}
