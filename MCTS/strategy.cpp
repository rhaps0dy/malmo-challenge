#include <cassert>
#include <cstdio>

#include "common.hpp"
#include "strategy.hpp"
#include "random.hpp"
#include "astar.hpp"

#include <chrono>
#include <iostream>
using namespace std::chrono;

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


StrategyChooser::StrategyChooser(Node &root, Float probs[],
								 size_t n_probs) : p_strats(n_probs) {
	assert(n_probs == 2);
	p_strats.emplace_back(make_pair(probs[0], unique_ptr<Strategy>(new StrategyRandom())));
	p_strats.emplace_back(make_pair(probs[1], unique_ptr<Strategy>(new StrategyPig(root))));
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

StrategyPig::StrategyPig(Node &root) : nodes() {
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	Node *node = astar_search<pig_position_f_cost<1>, pig_position_goal<1>,
							  false>(root.get_child(A_LEFT));
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	cout << "Astar took: " << duration_cast<microseconds>( t2 - t1 ).count() << endl;
	while(node != NULL) {
		nodes.push_back(node);
		node = node->parent;
	}
#ifndef NDEBUG
	for(auto it=nodes.rbegin(); it!=nodes.rend(); it++)
		(*it)->print();
#endif
	prev_node = nodes.rbegin();
}
Action StrategyPig::act(const Node &from) {
	if(prev_node == nodes.rend())
		return A_RIGHT;

	assert(from.t >= (*prev_node)->t);
	while(prev_node != nodes.rend() && (*prev_node)->t <= from.t)
		prev_node++;
	if(prev_node == nodes.rend())
		return A_RIGHT;
	assert((*prev_node)->t == from.t+1);
	return (*prev_node)->prev_action;
}
