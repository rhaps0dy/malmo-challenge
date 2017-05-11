#include <cassert>
#include <cmath>
#include <iostream>

#include "uct.hpp"
#include "strategy.hpp"
#include "random.hpp"

Float pig_chase_reward(const Node &n) {
	if(n.is_final) {
		if(n.pig_trapped()) return 25.;
		if(n.in_exit(0)) return 5.;
	}
	if(n.t%2 == 0)
		return -1.0;
	return 0.0;
}

static
Float uct_value(const Node &n, const Float c) {
	assert(n.n_visits > 0);
	return n.value_sum/n.n_visits + c*sqrt(2.*log(n.parent.n_visits)/n.n_visits);
}

static
size_t best_child_i(Node *current, const Float constant) {
	size_t n_best=0;
	size_t best[N_ACTIONS] = {99999};
	Float max_val = -INFINITY;

	for(size_t i=0; i<current->get_children().size(); i++) {
		auto &child = current->get_children().at(i);
		if(child.n_visits == 0) {
			if(!isnan(max_val))
				n_best = 0;
			max_val = NAN;
			best[n_best++] = i;
		}
		if(!isnan(max_val)) {
			auto val = uct_value(child, constant);
			if(val > max_val) {
				max_val = val;
				n_best = 1;
				best[0] = i;
			} else if (val == max_val) {
				best[n_best++] = i;
			}
		}
	}
	assert(n_best <= N_ACTIONS);
	return best[Random::uniform_int<size_t>(n_best-1)];
}

Node &simulate_path(Node &_current, Float constant) {
	const Strategy strategy = StrategyChooser::get().random_strat();
	Node *current = &_current;
	while(!current->is_final) {
		const int role = current->t%2;
		if(role == 0) {
			auto best_i = best_child_i(current, constant);
			current = &current->get_children().at(best_i);
		} else {
			Action a = strategy(*current);
			current = &current->get_children().at(static_cast<int>(a));
		}
	}
	return *current;
}

Action uct_best_action(Node &root, int budget, RewardFun reward_f, Float constant)
{
	assert(budget>0);
	while(budget--) {
		Node &end = simulate_path(root, constant);
		Node *n = &end;
		Float value = 0.0;
		for(;;) {
			value += reward_f(*n);
			n->value_sum += value;
			n->n_visits += 1;
			if(n == &root) break;
			n = &n->parent;
		}
	}
	auto best_i = best_child_i(&root, 0.0);
	return root.get_children()[best_i].prev_action;
}

Action ffi_best_action(int x0, int y0, Direction d0, int x1, int y1,
					   Direction d1, int P_x, int P_y, int budget, Float c,
					   Float strat_probs[], size_t n_strats)
{
	Node root(x0, y0, d0, x1, y1, d1, P_x, P_y);
	StrategyChooser::get().update_probabilities(strat_probs, n_strats);
	const Action a = uct_best_action(root, budget, pig_chase_reward, c);

#ifndef NDEBUG
	root.print();
	vector<Node *> ns;
	ns.push_back(&root);
	for(size_t j=0; ns.size() < 20; j++) {
		if(ns[j]->is_final) continue;
		for(Node &c: ns[j]->get_children())
			if(c.n_visits > 0)
				ns.push_back(&c);
	}
	for(Node *n: ns) {
		cerr << "t = " << n->t << ", a = " << n->prev_action << ", value = " << uct_value(*n, 0) << endl;
	}
#endif

	return a;
}


#ifndef NDEBUG
// test
int main() {
	Float ps[2] = {0., 1.};
	constexpr int budget=100000;
	assert(ffi_best_action(2, 3, D_NORTH, 4, 3, D_EAST, 6, 1, budget, 2.0, ps, 2) == 0);
	ps[0] = 1;
	ps[1] = 0;
	assert(ffi_best_action(2, 3, D_EAST, 6, 3, D_SOUTH, 5, 3, budget, 2.0, ps, 2) == 2);
	return 0;
}
#endif
