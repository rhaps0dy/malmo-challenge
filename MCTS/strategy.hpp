#pragma once
#include <random>

#include "common.hpp"

using namespace std;

typedef Action (*Strategy)(Player, Player, int, Pig);

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

Action StrategyLeft(Player p1, Player p2, int t, Pig pig);
Action StrategyRight(Player p1, Player p2, int t, Pig pig);
Action StrategyFront(Player p1, Player p2, int t, Pig pig);
