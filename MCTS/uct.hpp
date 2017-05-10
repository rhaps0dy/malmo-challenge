#pragma once

#include <vector>
#include <utility>

#include "common.hpp"
#include "strategy.hpp"
#include "node.hpp"

using namespace std;

class UCTNode : public Node<UCTNode> {
protected:
	Float value_sum;
	int n_visits;

public:
	UCTNode(const UCTNode *_parent, int x, int y, Direction d, Action _prev_a);
	UCTNode(int x0, int y0, Direction d0, int x1, int y1, Direction d1,
			int _P_x, int _P_y);
	Float value(const Float c) const;
	UCTNode *simulate_path(Float constant=1.);
	virtual ~UCTNode() {};
};
