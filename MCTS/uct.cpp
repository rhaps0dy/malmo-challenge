#include "uct.hpp"

UCTNode::UCTNode(const UCTNode &_parent, int x, int y, Direction d, Action _prev_a) :
	Node(_parent, x, y, d, _prev_a), value_sum(0.0), n_visits(0) {}

UCTNode::UCTNode(int x0, int y0, Direction d0, int x1, int y1, Direction d1,
				 int _P_x, int _P_y) :
	Node(x0, y0, d0, x1, y1, d1, _P_x, _P_y) {}

Float UCTNode::value(const Float c) const {
	if(is_final) {
		Float v = -(t/2);
		if(pig_trapped()) return v+25.;
		if(in_exit(0)) return v+5.;
		return v;
	}
	return value_sum/n_visits + c*sqrt(2.*log(parent.n_visits)/n_visits);
}

UCTNode &UCTNode::simulate_path(Float constant) {
	const Strategy strategy = StrategyChooser::get().random_strat();
	UCTNode *current = this;
	while(!current->is_final) {
		const int role = current->t%2;
		if(role == 0) {
			size_t i=0, best_i=0;
			Float max_val = current->get_children()[0].value(constant);
			for(i=0; i<current->get_children().size(); i++) {
				auto val = current->get_children()[i].value(constant);
				if(val > max_val) {
					max_val = val;
					best_i = i;
				}
			}
			current = &current->get_children()[i];
		} else {
			Action a = strategy(current->ps[0], current->ps[1], current->t,
								 current->pig);
			current = &current->get_children()[(int)a];
		}
	}
	return *current;
}
