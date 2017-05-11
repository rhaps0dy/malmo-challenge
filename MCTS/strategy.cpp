#include <cassert>

#include "common.hpp"
#include "strategy.hpp"
#include "random.hpp"

Strategy StrategyChooser::random_strat()
{
	Float f = Random::uniform<Float>();
	const StrategyChooser &sc = get();
	Float sum = 0.0;
	for(size_t i=0;;i++) {
		sum += sc.p1_strats.at(i).first;
		if(sum > f)
			return sc.p1_strats.at(i).second;
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

Action StrategyRandom(Node &start) {
	static default_random_engine generator(
		chrono::system_clock::now().time_since_epoch().count());
	static uniform_int_distribution<size_t> uniform2(0, 1);
	static uniform_int_distribution<size_t> uniform3(0, 2);
	if(start.get_children().size() == 2)
		return static_cast<Action>(uniform2(generator));
	return static_cast<Action>(uniform3(generator));
}


StrategyChooser::StrategyChooser() :
	p1_strats{{1./2., StrategyLeft}, {1., StrategyRandom}} {}

void StrategyChooser::update_probabilities(Float probs[], size_t n_probs) {
	StrategyChooser &sc = get();
	assert(n_probs == sc.p1_strats.size());
	for(size_t i=0; i<n_probs; i++) {
		sc.p1_strats[i].first = probs[i];
	}
	// make sure probabilities sum to more than 1
	sc.p1_strats[n_probs-1].first += 0.1;
}
