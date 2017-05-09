#pragma once

#include "common.hpp"

struct UCTNode : public Node {
	Float value_sum;
	int n_visits;
	bool visited_all_children;

	UCTNode(const Node &_parent, int x, int y, Direction d, Action _prev_a):
		Node(_parent, x, y, d, _prev_a), value_sum(0.0), n_visits(0), visited_all_children(false) {}

	UCTNode(int x0, int y0, Direction d0, int x1, int y1, Direction d1, int _P_x, int _P_y):
		Node(x0, y0, d0, x1, y1, d1, _P_x, _P_y), value_sum(0.0), n_visits(0), visited_all_children(false) {}

	/*
	const Float value(const Float c) {
		if(is_final) {
			Float v = -(t/2);
			if(ps[0].x ==
			return v;
		}
		return value_sum/n_visits + c*sqrt(2.*log(parent.n_visits)/n_visits);
	}*/

	// Node &simulate_path(
};
