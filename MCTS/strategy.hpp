#pragma once

#include "common.hpp"
#include "node.hpp"

#include <utility>
#include <vector>

using namespace std;

typedef Action(*Strategy)(const Node &);

class StrategyChooser {
	vector<pair<Float, Strategy> > p_strats;
public:
	StrategyChooser(Float probs[], size_t n_probs);
	Strategy random_strat(size_t &i);
	void update_probabilities(Float probs[], size_t n_probs);
};

Action StrategyRandom(const Node &from);
Action StrategyPig(const Node &from);
