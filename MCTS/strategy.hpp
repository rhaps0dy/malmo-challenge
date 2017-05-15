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
	StrategyChooser(Node &root, Float probs[], size_t n_probs);
	Strategy &random_strat();
	void update_probabilities(Float probs[], size_t n_probs);
};

struct StrategyRandom : public Strategy {
	virtual Action act(const Node &from);
//	virtual void reset() {}
};

struct _t_action { int t; Action a; };

struct StrategyPig : public Strategy {
protected:
	vector<_t_action> nodes;
	vector<_t_action>::reverse_iterator prev_node;
public:
	StrategyPig(Node &root);
	virtual Action act(const Node &from);
	virtual void reset() { prev_node = nodes.rbegin(); }
};
