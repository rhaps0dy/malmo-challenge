#pragma once

#include <map>
#include <array>
#include <memory>
#include <memory>

#include "common.hpp"

using namespace std;

typedef int NodeSeri;

struct Node {
	Node *parent;
	Action prev_action;
	bool is_final;
	int value;

	// State of the world
	int t;
	Pig pig;
	array<Player, 2> ps;

private:
	array<unique_ptr<Node>, N_ACTIONS> children;
	void check_if_final();
public:
	const static array<Action, N_ACTIONS> actions;

	Node(Node *_parent);
	Node(int x0, int y0, Direction d0, int x1, int y1, Direction d1,
		 int _P_x, int _P_y);
	void print() const;
	NodeSeri get_serialization() const;
	bool operator<(const Node &rhs) {
		return get_serialization() < rhs.get_serialization();
	}
	bool pig_trapped() const;
	bool in_exit(int role) const;
	Node* get_child(const Action action);
	void make_child(const Action action, const bool pig_move=false);
};


