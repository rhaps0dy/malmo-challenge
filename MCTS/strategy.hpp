#pragma once

#include <random>

#include "common.hpp"
#include "node.hpp"

using namespace std;

typedef Action (*Strategy)(Node&);

class StrategyChooser {
	default_random_engine generator;
	uniform_real_distribution<Float> uniform;
	vector<pair<Float, Strategy> > p1_strats;

public:
	static StrategyChooser &get() {
		static StrategyChooser instance;
		return instance;
	}
	StrategyChooser(StrategyChooser const&) = delete;
	void operator=(StrategyChooser const&) = delete;

	StrategyChooser();
	Strategy random_strat();
};

Action StrategyLeft(Node &start);
Action StrategyRight(Node &start);
Action StrategyFront(Node &start);
