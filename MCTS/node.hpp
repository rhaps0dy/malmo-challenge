#pragma once

#include <map>
#include <array>

#include "common.hpp"

using namespace std;

typedef int NodeSeri;

struct Node {
	Node *parent;
	Action prev_action;
	bool is_final;

	// For MCTS
	array<Float, N_ACTIONS> value_sum;
	array<int, N_ACTIONS> n_visits;
	int total_n_visits;

	// State of the world
	int t;
	Pig pig;
	array<Player, 2> ps;

public:
	map<NodeSeri, Node> children;
	array<Node*, N_ACTIONS> children_nopigmove;
private:
	void check_if_final();
public:
	const static array<Action, N_ACTIONS> actions;

	Node(Node *_parent);
	Node(Node *_parent, int x, int y, Direction d, Action _prev_a);
	Node(int x0, int y0, Direction d0, int x1, int y1, Direction d1,
		 int _P_x, int _P_y);
	void print() const;
	NodeSeri get_serialization() const;
	bool operator<(const Node &rhs) {
		return get_serialization() < rhs.get_serialization();
	}
	bool pig_trapped() const;
	bool in_exit(int role) const;
	Node *get_child(const Action action, const bool pig_move=false);
	void make_child(const Action action, const bool pig_move=false);
};


