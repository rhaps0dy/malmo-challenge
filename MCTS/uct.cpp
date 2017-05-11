#include "uct.hpp"
#include "strategy.hpp"

Float uct_value(const Node &n, const Float c) {
	if(n.is_final) {
		Float v = -(n.t/2);
		if(n.pig_trapped()) return v+25.;
		if(n.in_exit(0)) return v+5.;
		return v;
	}
	return n.value_sum/n.n_visits + c*sqrt(2.*log(n.parent.n_visits)/n.n_visits);
}

Node &simulate_path(Node &_current, Float constant) {
	const Strategy strategy = StrategyChooser::get().random_strat();
	Node *current = &_current;
	while(!current->is_final) {
		const int role = current->t%2;
		if(role == 0) {
			size_t i=0, best_i=0;
			Float max_val = uct_value(current->get_children().at(0), constant);
			for(i=0; i<current->get_children().size(); i++) {
				auto val = uct_value(current->get_children().at(i), constant);
				if(val > max_val) {
					max_val = val;
					best_i = i;
				}
			}
			current = &current->get_children().at(best_i);
		} else {
			Action a = strategy(*current);
			current = &current->get_children().at(static_cast<int>(a));
		}
	}
	return *current;
}
