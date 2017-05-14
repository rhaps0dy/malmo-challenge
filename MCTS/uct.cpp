#include <cassert>
#include <cmath>
#include <iostream>

#include "uct.hpp"
#include "strategy.hpp"
#include "random.hpp"

Float pig_chase_reward(const Node &n) {
	if(n.is_final) {
		if(n.pig_trapped()) return 24.;
		if(n.in_exit(0)) return 4.;
	}
	if(n.t%2 == 1)
		return -1.0;
	return 0.0;
}

static
Float uct_value(const Node &n, const size_t i, const Float c) {
	assert(n.n_visits.at(i) > 0);
	return n.value_sum[i]/n.n_visits[i] + c*sqrt(2.*log(n.total_n_visits)/n.n_visits[i]);
}

static
Action best_child_action(Node *current, const Float constant) {
	size_t n_best=0;
	Action best[N_ACTIONS];
	Float max_val = -INFINITY;

	for(size_t i=0; i<N_ACTIONS; i++) {
		if(current->n_visits.at(i) == 0) {
			if(!isnan(max_val))
				n_best = 0;
			max_val = NAN;
			best[n_best++] = static_cast<Action>(i);
		} else if(!isnan(max_val)) {
			auto val = uct_value(*current, i, constant);
			if(val > max_val) {
				max_val = val;
				n_best = 1;
				best[0] = static_cast<Action>(i);
			} else if (val == max_val) {
				best[n_best++] = static_cast<Action>(i);
			}
		}
	}
	return best[Random::uniform_int<size_t>(n_best-1)];
}

Node &simulate_path(Node &_current, Float constant, StrategyChooser &sc) {
	Strategy &strategy = sc.random_strat();
	Node *current = &_current;
	while(!current->is_final) {
		const int role = current->t%2;
		if(role == 0) {
			auto best_a = best_child_action(current, constant);
			current = &current->get_child(best_a, false);
		} else {
			Action a = strategy.act(*current);
			current = &current->get_child(a, true);
		}
	}
	return *current;
}

Action uct_best_action(Node &root, int budget, RewardFun reward_f,
					   Float constant, StrategyChooser &sc)
{
	assert(budget>0);
	while(budget--) {
		Node *n = &simulate_path(root, constant, sc);
		Float value = 0.0;
		do {
			value += reward_f(*n);
			n->total_n_visits += 1;
			size_t i = static_cast<size_t>(n->prev_action);
			n = &n->parent;
			n->value_sum[i] += value;
			n->n_visits[i] += 1;
		} while(n != &root);
	}
	return best_child_action(&root, 0.0);
}

Action ffi_best_action(int x0, int y0, Direction d0, int x1, int y1,
					   Direction d1, int P_x, int P_y, int budget, Float c,
					   Float strat_probs[], size_t n_probs)
{
	Node root(x0, y0, d0, x1, y1, d1, P_x, P_y);
	StrategyChooser sc(root, strat_probs, n_probs);
	const Action a = uct_best_action(root, budget, pig_chase_reward, c, sc);

#ifndef NDEBUG
	root.print();
	vector<Node *> ns;
	ns.push_back(&root);
	for(size_t j=0; ns.size() < 20; j++) {
		if(ns[j]->is_final) continue;
		for(auto c: ns[j]->children)
			if(c.second.total_n_visits > 0)
				ns.push_back(&c.second);
	}
	for(Node *n: ns) {
		cerr << "t = " << n->t << ", a = " << n->prev_action << ", value = " << uct_value(n->parent, static_cast<size_t>(n->prev_action), 0) << endl;
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
