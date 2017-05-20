#include <cassert>
#include <cstdio>

#include "common.hpp"
#include "strategy.hpp"
#include "random.hpp"
#include "path_cache.hpp"

#include <iostream>

Strategy &StrategyChooser::random_strat()
{
	Float f = Random::uniform<Float>();
	Float sum = 0.0;
	for(size_t i=0;;i++) {
		sum += p_strats.at(i).first;
		if(sum > f)
			return *p_strats.at(i).second;
	}
	// probabilities should sum to 1 so this shouldn't be reached
	assert(false);
}


StrategyChooser::StrategyChooser(Float probs[], size_t n_probs) : p_strats() {
	assert(n_probs == 2);
	p_strats.emplace_back(make_pair(probs[0], unique_ptr<Strategy>(new StrategyRandom())));
	p_strats.emplace_back(make_pair(probs[1], unique_ptr<Strategy>(new StrategyPig())));
}

void StrategyChooser::update_probabilities(Float probs[], size_t n_probs) {
	assert(n_probs == p_strats.size());
	for(size_t i=0; i<n_probs; i++) {
		p_strats[i].first = probs[i];
	}
	// make sure probabilities sum to more than 1
	p_strats[n_probs-1].first += 1;
}

Action StrategyRandom::act(const Node &from) {
	static default_random_engine &generator = Random::get_generator();
	static uniform_int_distribution<size_t> uniform3(0, 2);
	return static_cast<Action>(uniform3(generator));
}

Action StrategyPig::act(const Node &from) {
	if(PathCache::get_cost<1, OBJECTIVE_PIG>(from) == 1)
		return A_RIGHT;
	return PathCache::get_action<1, OBJECTIVE_PIG>(from);
}
