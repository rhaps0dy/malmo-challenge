#include <cassert>

#include "common.hpp"
#include "strategy.hpp"

Strategy StrategyChooser::random_strat()
{
	Float f = uniform(generator);
	Float sum = 0.0;
	for(size_t i=0;;i++) {
		sum += p1_strats.at(i).first;
		if(sum > f)
			return p1_strats.at(i).second;
	}
	// probabilities should sum to 1 so this shouldn't be reached
	assert(false);
}

Action StrategyLeft(Player p1, Player p2, int t, Pig pig) { return A_LEFT; }
Action StrategyRight(Player p1, Player p2, int t, Pig pig) { return A_RIGHT; }
Action StrategyFront(Player p1, Player p2, int t, Pig pig) { return A_FRONT; }

StrategyChooser::StrategyChooser() : generator(), uniform(0.0, 1.0), p1_strats()
{
	p1_strats.push_back(make_pair(1./3., StrategyLeft));
	p1_strats.push_back(make_pair(1./3., StrategyRight));
	p1_strats.push_back(make_pair(1.-p1_strats.at(1).first-p1_strats.at(0).first, StrategyRight));
}
