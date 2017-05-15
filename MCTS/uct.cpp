#include <cassert>
#include <cmath>
#include <iostream>
#include <cstdio>

#include "uct.hpp"
#include "strategy.hpp"
#include "random.hpp"

#include <chrono>
#include <unordered_map>
using namespace std::chrono;

// The last one here contains the sum of the other 3
static unordered_map<NodeSeri, array<int, N_ACTIONS+1> > _n_visits;

static unordered_map<NodeSeri, array<Float, N_ACTIONS> > _value_sum;

static
Action best_child_action(array<int, N_ACTIONS+1> &n_visits,
						 array<Float, N_ACTIONS> &value_sum,
						 const Float constant) {
	size_t n_best=0;
	Action best[N_ACTIONS];
	Float max_val = -INFINITY;

	for(size_t i=0; i<N_ACTIONS; i++) {
		if(n_visits[i] == 0) {
			if(!isnan(max_val))
				n_best = 0;
			max_val = NAN;
			best[n_best++] = static_cast<Action>(i);
		} else if(!isnan(max_val)) {
			Float val = value_sum[i]/n_visits[i] +
				constant*sqrt(2.*log(n_visits[N_ACTIONS])/n_visits[i]);
			if(val > max_val) {
				max_val = val;
				n_best = 1;
				best[0] = static_cast<Action>(i);
			} else if (val == max_val) {
				best[n_best++] = static_cast<Action>(i);
			}
		}
	}
	assert(n_best > 0);
	return best[Random::uniform_int<size_t>(n_best-1)];
}

#define TIME_SERIALIZATION(node) (node.get_serialization()*(MAX_T+1) + node.t)
static
void simulate_path(Node current, Float constant, StrategyChooser &sc) {
	static vector<Float*> values;
	values.clear();

	Strategy &strategy = sc.random_strat();
	strategy.reset();

	while(!current.is_final) {
		if(current.t%2 == 0) {
			const NodeSeri ns = TIME_SERIALIZATION(current);
			auto &n_visits = _n_visits.insert({ns, {{0}}}).first->second;
			auto &value_sum = _value_sum.insert({ns, {{0.0}}}).first->second;
			const Action best_a = best_child_action(n_visits, value_sum,
													constant);
			values.push_back(&value_sum[best_a]);
			n_visits[best_a]++;
			n_visits[N_ACTIONS]++;
			current.make_child(best_a, false);
		} else {
			const Action a = strategy.act(current);
			current.make_child(a, true);
		}
	}
	const Float value = (current.value_sum[0] + 25.) / 50.;
	for(auto v: values)
		(*v) += value;
}

Action uct_best_action(Node &root, int budget,
					   Float constant, StrategyChooser &sc)
{
	assert(budget>0);
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	while(budget--) {
		simulate_path(root, constant, sc);
		if(budget % 1000 == 0) {
			high_resolution_clock::time_point t2 = high_resolution_clock::now();
			cout << "1000 paths took: " << duration_cast<microseconds>( t2 - t1 ).count() << endl;
			t1 = high_resolution_clock::now();
		}
	}
	const NodeSeri root_seri = TIME_SERIALIZATION(root);
	auto a = _n_visits.find(root_seri);
	auto b = _value_sum.find(root_seri);
	return best_child_action(a->second,
							 b->second, 0.0);
}
#undef TIME_SERIALIZATION

Action ffi_best_action(int x0, int y0, Direction d0, int x1, int y1,
					   Direction d1, int P_x, int P_y, int budget, Float c,
					   Float strat_probs[], size_t n_probs)
{
	Node root(x0, y0, d0, x1, y1, d1, P_x, P_y);
	StrategyChooser sc(root, strat_probs, n_probs);
	_value_sum.clear();
	_n_visits.clear();
	root.print();
	Action a = uct_best_action(root, budget, c, sc);

	Node *next = root.get_child(a)->get_child(A_RIGHT);
	next->print();
	a = uct_best_action(*next, budget, c, sc);

	next = next->get_child(a);
	next->print();

#ifndef NDEBUG
	vector<Node *> ns;
	ns.push_back(&root);
	for(size_t j=0; j < ns.size() && ns.size() < 20; j++) {
		for(Action a: Node::actions) {
			Node *c = ns[j]->get_child(a);
			if(c->parent->n_visits[a] > 0)
				ns.push_back(c);
		}
	}
	for(Node *n: ns) {
		Float value = 0.0;
		for(Float v: _value_sum[n->get_serialization()])
			value += v;
		value /= _n_visits[n->get_serialization()][N_ACTIONS];
		cerr << "t = " << n->t << ", a = " << n->prev_action << ", value = " <<
			value << endl;
	}
#endif

	return a;
}


//#ifndef NDEBUG
// test
int main() {
	Float ps[2] = {0., 1.};
	constexpr int budget=10000;
	if(ffi_best_action(2, 3, D_NORTH, 4, 3,
					   D_EAST, 6, 1, budget, 2.0, ps, 2) != A_FRONT) {
		cout << "Failed 1st\n";
	}
	if(ffi_best_action(2, 3, D_EAST, 6, 3,
						   D_SOUTH, 5, 3, budget, 2.0, ps, 2) != A_FRONT) {
		cout << "Failed 2nd\n";
	}
	return 0;
}
//#endif
