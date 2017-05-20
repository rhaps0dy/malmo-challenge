#pragma once

#include "common.hpp"
#include "node.hpp"

#include <utility>
#include <vector>

using namespace std;

struct Strategy {
	virtual Action act(const Node &from) = 0;
	virtual void reset() {}
};

class StrategyChooser {
	vector<pair<Float, unique_ptr<Strategy> > > p_strats;
public:
	StrategyChooser(Float probs[], size_t n_probs);
	Strategy &random_strat();
	void update_probabilities(Float probs[], size_t n_probs);
};

struct StrategyRandom : public Strategy {
	virtual Action act(const Node &from);
};

struct StrategyPig : public Strategy {
public:
	virtual Action act(const Node &from);
};
