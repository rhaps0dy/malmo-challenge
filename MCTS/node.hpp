#pragma once

#include <vector>

#include "common.hpp"

using namespace std;

struct Node {
	Node &parent;
	Action prev_action;
	bool is_final;

	// For MCTS
	Float value_sum;
	int n_visits;

	// State of the world
	int t;
	Pig pig;
	Player ps[2];
protected:
	vector<Node> children;

public:
	Node(Node &_parent, int x, int y, Direction d, Action _prev_a);
	Node(int x0, int y0, Direction d0, int x1, int y1, Direction d1,
		 int _P_x, int _P_y);
	vector<Node> &get_children();
	bool pig_trapped() const;
	bool in_exit(int role) const;
	void print() const;
	int get_serialization() const;
};
