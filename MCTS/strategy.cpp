#include <cassert>
#include <chrono>

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

Action StrategyLeft(Node &start) { return A_LEFT; }
Action StrategyRight(Node &start) { return A_RIGHT; }
Action StrategyFront(Node &start) {
	if(start.get_children().size() > 2)
		return A_FRONT;
	return A_LEFT;
}


StrategyChooser::StrategyChooser() :
	generator(chrono::system_clock::now().time_since_epoch().count()),
	uniform(0.0, 1.0), p1_strats{
	{1./3., StrategyLeft}, {1./3., StrategyRight}, {1., StrategyFront}} {}
