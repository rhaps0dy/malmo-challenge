#include <cassert>
#include <cstdio>

#include "common.hpp"
#include "strategy.hpp"
#include "random.hpp"
#include "path_cache.hpp"

#include <iostream>

Strategy StrategyChooser::random_strat(size_t &i)
{
	Float f = Random::uniform<Float>();
	Float sum = 0.0;
	for(i=0;;i++) {
		sum += p_strats.at(i).first;
		if(sum > f)
			return p_strats.at(i).second;
	}
	// probabilities should sum to 1 so this shouldn't be reached
	assert(false);
}


StrategyChooser::StrategyChooser(Float probs[], size_t n_probs) : p_strats() {
	assert(n_probs == 2);
	p_strats.emplace_back(make_pair(probs[0], StrategyPig));
	p_strats.emplace_back(make_pair(probs[1], StrategyRandom));
}

void StrategyChooser::update_probabilities(Float probs[], size_t n_probs) {
	assert(n_probs == p_strats.size());
	for(size_t i=0; i<n_probs; i++) {
		p_strats[i].first = probs[i];
	}
	// make sure probabilities sum to more than 1
	p_strats[n_probs-1].first += 1;
}

Action StrategyRandom(const Node &from) {
	static default_random_engine &generator = Random::get_generator();
	static uniform_int_distribution<size_t> uniform3(0, 2);
	return static_cast<Action>(uniform3(generator));
}

Action StrategyPig(const Node &from) {
	if(PathCache::data<1, OBJECTIVE_PIG, TYPE_COST>(from) == 1)
		return A_RIGHT;
	return static_cast<Action>(PathCache::data<1, OBJECTIVE_PIG, TYPE_ACTION>(from));
}
