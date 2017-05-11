#pragma once

#include "common.hpp"
#include "node.hpp"

using namespace std;

typedef Action (*Strategy)(Node&);

class StrategyChooser {
	vector<pair<Float, Strategy> > p1_strats;

public:
	static StrategyChooser &get() {
		static StrategyChooser instance;
		return instance;
	}
	StrategyChooser(StrategyChooser const&) = delete;
	void operator=(StrategyChooser const&) = delete;

	StrategyChooser();
	static Strategy random_strat();
	static void update_probabilities(Float probs[], size_t n_probs);
};

Action StrategyLeft(Node &start);
Action StrategyRight(Node &start);
Action StrategyFront(Node &start);
