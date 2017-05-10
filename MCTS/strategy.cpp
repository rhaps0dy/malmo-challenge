#include <cassert>

#include "common.hpp"
#include "strategy.hpp"

Strategy StrategyChooser::random_strat()
{
	Float f = uniform(generator);
	Float sum = 0.0;
	for(size_t i=0;;i++) {
		sum += p1_strats[i].first;
		if(sum > f)
			return p1_strats[i].second;
	}
	// probabilities should sum to 1 so this shouldn't be reached
	assert(false);
}

Action StrategyLeft(Player p1, Player p2, int t, Pig pig) { return A_LEFT; }
Action StrategyRight(Player p1, Player p2, int t, Pig pig) { return A_RIGHT; }
Action StrategyFront(Player p1, Player p2, int t, Pig pig) { return A_FRONT; }
