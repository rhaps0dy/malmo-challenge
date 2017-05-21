#include <cassert>
#include <cmath>
#include <iostream>
#include <cstdio>

#include "uct.hpp"
#include "strategy.hpp"
#include "random.hpp"
#include "path_cache.hpp"

#include <chrono>
#include <unordered_map>
using namespace std::chrono;

// The last one here contains the sum of the other 3
static unordered_map<NodeSeri, array<int, N_ACTIONS+1> > _n_visits;

static unordered_map<NodeSeri, array<Float, N_ACTIONS> > _value_sum;

// Returns the best action to take from the node given by n_visits and
// value_sum, or the node without any visits if it exists. This is equivalent to
// taking one step in the UCT tree policy.
//
// The paremeter `action_never_taken` is assigned value `true` if the action
// returned has never been taken from the given node.
static Action
tree_policy_action(std::array<int, N_ACTIONS+1> &n_visits,
				   array<Float, N_ACTIONS> &value_sum, const Float constant,
				   bool &action_never_taken) {
	size_t n_best=0;
	Action best[N_ACTIONS];
	Float max_val = -INFINITY;

	for(size_t i=0; i<N_ACTIONS; i++) {
		if(n_visits[i] == 0) {
			// Is there an action that has never been taken?
			if(!isnan(max_val)) {
				// If so, and this is the first action we see that hasn't been
				// taken, discard other actions to randomly choose
				n_best = 0;
			}
			max_val = NAN;
			best[n_best++] = static_cast<Action>(i);
			action_never_taken = true;
		} else if(!isnan(max_val)) {
			// Otherwise, if no other action has never been taken,
			// calculate the UCT value
			Float val = value_sum[i]/n_visits[i] +
				constant*sqrt(2.*log(n_visits[N_ACTIONS])/n_visits[i]);

			// And if it's the highest, add this node to the list of equally
			// best nodes.
			if(val > max_val) {
				max_val = val;
				n_best = 1;
				best[0] = static_cast<Action>(i);
			} else if (val == max_val) {
				// Ties are broken uniformly randomly
				best[n_best++] = static_cast<Action>(i);
			}
		}
	}
	assert(n_best > 0);
	// Break ties uniformly randomly
	return best[Random::uniform_int<size_t>(n_best-1)];
}

static Action
default_policy_action(const Node& node, const int option) {
    if (option == 0) {
        return static_cast<Action>(PathCache::data<0, OBJECTIVE_EXIT, TYPE_ACTION>(node));
    } else {
        return static_cast<Action>(PathCache::data<0, OBJECTIVE_CORNER_PIG, TYPE_ACTION>(node));
	}
}

#define TIME_SERIALIZATION(node) (node.get_serialization()*(MAX_T+1) + node.t)
static
void simulate_path(Node current, Float constant, StrategyChooser &sc) {
	// contains pointers to the <node, action> 's sum of values that we have
	// passed through to get to the final node.
	static vector<Float*> values;
	values.clear();

	// A strategy (aka a policy) that the other agent takes, drawn from its
	// strategy mix
	Strategy &strategy = sc.random_strat();
	strategy.reset();

	// Whether our agent takes the tree policy or the default policy.
	bool using_default_policy = false;

	// Whether the default policy involves going to the exit or to the pig
	const int option_default = Random::uniform_int<int>(1);

	while(!current.is_final) {
		if(current.t%2 == 0) {
			// Get key of this node in the _n_visits and _value_sum maps. Then
			// fetch (or create if they don't exist) this node's arrays in them.
			const NodeSeri ns = TIME_SERIALIZATION(current);
			auto &n_visits = _n_visits.insert({ns, {{0,0,0,0}}}).first->second;
			auto &value_sum = _value_sum.insert({ns, {{0.0,0.0,0.0}}}).first->second;

			// Choose the immediate child according to the tree policy or the default
			// policy. Note that we don't choose the end result of the tree or
			// default policy, only the child of this node according to that
			// policy.
			// This also modifies the `using_default_policy` variable, to set it
			// to `true` if the tree policy should finish.
			const Action best_a =
				(using_default_policy ?
				 default_policy_action(current, option_default) :
				 tree_policy_action(n_visits, value_sum, constant,
									using_default_policy));

			// Record this ancestor's <node, value> pair for later backup
			values.push_back(&value_sum[best_a]);

			// Add 1 to the number of visits of <node,action> pair. This is
			// usually done when backing up, but it's faster to do it here.
			n_visits[best_a]++;
			n_visits[N_ACTIONS]++;

			current.make_child(best_a, false);
		} else {
			// Turn for the other agent, it takes an action according to its
			// policy
			const Action a = strategy.act(current);
			current.make_child(a, true);
		}
	}
	// Backup rule: value is the reward obtained in the leaf. Then we go up
	// through this leaf's ancestors and we add the value to them.
	const Float value = static_cast<Float>(current.value + 25) / 50.;
	for(auto v: values)
		(*v) += value;
}

Action uct_best_action(Node &root, int budget,
					   Float constant, StrategyChooser &sc)
{
	assert(budget>0);
	if(root.is_final)
		return A_RIGHT;
#ifndef NPROFILE
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
#endif
	while(budget--) {
		simulate_path(root, constant, sc);
#ifndef NPROFILE
		if(budget % 1000 == 0) {
			high_resolution_clock::time_point t2 = high_resolution_clock::now();
			cout << "1000 paths took: " <<
				duration_cast<microseconds>(t2 - t1).count() << endl;
			t1 = high_resolution_clock::now();
		}
#endif
	}
	const NodeSeri root_seri = TIME_SERIALIZATION(root);
	auto a = _n_visits.find(root_seri);
	auto b = _value_sum.find(root_seri);
	bool _dummy;
	return tree_policy_action(a->second, b->second, 0.0, _dummy);
}
#undef TIME_SERIALIZATION

Action ffi_best_action(int budget, Float c,
					   Float strat_probs[], size_t n_probs,
					   int P_y, int P_x, int y1, int x1, Direction d1,
					   int y0, int x0, Direction d0)
{
	static StrategyChooser sc(strat_probs, n_probs);
	Node root(x0, y0, d0, x1, y1, d1, P_x, P_y);
	sc.update_probabilities(strat_probs, n_probs);
	_value_sum.clear();
	_n_visits.clear();
#ifdef NDEBUG
	return uct_best_action(root, budget, c, sc);
#else
	Action a = uct_best_action(root, budget, c, sc);
	root.print();

	root.make_child(a);
	root.make_child(A_RIGHT);
	root.print();
	a = uct_best_action(root, budget, c, sc);

	root.make_child(a);
	root.print();

	vector<Node> ns;
	ns.push_back(root);
	for(size_t j=0; j < ns.size() && ns.size() < 20; j++) {
		for(Action a: Node::actions) {
			Node n = ns[j];
			n.make_child(a);
			if(_n_visits[n.get_serialization()][N_ACTIONS] > 0)
				ns.push_back(n);
		}
	}
	for(Node &n: ns) {
		Float value = 0.0;
		for(Float v: _value_sum[n.get_serialization()])
			value += v;
		value /= _n_visits[n.get_serialization()][N_ACTIONS];
		cerr << "t = " << n.t << ", a = " << n.prev_action << ", value = " <<
			value << endl;
	}
	return a;
#endif
}


#ifndef NHAVE_MAIN
// test
int main() {
	Float ps[2] = {0., 1.};
	constexpr int budget=10000;
	if(ffi_best_action(budget, 2.0, ps, 2,
					   1, 6, 3, 4, D_EAST, 3, 2, D_NORTH) !=
#ifndef NDEBUG
	   A_FRONT
#else
	   A_LEFT
#endif
		) {
		cout << "Failed 1st\n";
	} else {
		cout << "Succeeded 1st\n";
	}
	if(ffi_best_action(budget, 2.0, ps, 2,
					   3, 5, 3, 6, D_SOUTH, 3, 2, D_EAST) != A_FRONT) {
		cout << "Failed 2nd\n";
	} else {
		cout << "Succeeded 2nd\n";
	}
	return 0;
}
#endif

void ffi_print_state(int P_y, int P_x, int y1, int x1, Direction d1,
					 int y0, int x0, Direction d0) {
	Node n(x0, y0, d0, x1, y1, d1, P_x, P_y);
	n.print();
}
