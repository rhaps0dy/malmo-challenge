#include <cassert>
#include <iostream>
#include <cstdio>
#include <utility>
#include <set>

#include "node.hpp"


const array<Action, N_ACTIONS> Node::actions = {{A_FRONT, A_LEFT, A_RIGHT}};

Node::Node(Node *_parent, int x, int y, Direction d, Action _prev_a) :
	parent(_parent), prev_action(_prev_a), is_final(false), value_sum{{0.,0.,0.}},
	n_visits{{0,0,0}}, total_n_visits(0), t(parent->t+1), pig(parent->pig),
	ps(parent->t%2 == 0 ?
	   array<Player, 2>{{Player(x, y, d), parent->ps[1]}} :
	   array<Player, 2>{{parent->ps[0], Player(x, y, d)}}),
	children(),
	children_nopigmove{{NULL, NULL, NULL}}
{
	assert(t<=MAX_T);
	const int role = t % 2;
	// role == 1 -> the environment has the final move
	if(role==1 && (pig_trapped() || in_exit(1) || in_exit(0) || t>=MAX_T))
		is_final = true;
}

Node::Node(int x0, int y0, Direction d0, int x1, int y1, Direction d1,
		int _P_x, int _P_y) :
	parent(NULL), prev_action(A_FRONT), is_final(false), value_sum{{0.,0.,0.}},
	n_visits{{0,0,0}}, total_n_visits(0), t(0), pig{_P_x, _P_y},
	ps{{Player(x0, y0, d0), Player(x1, y1, d1)}},
	children(),
	children_nopigmove{{NULL, NULL, NULL}}
{
}

Node *Node::get_child(const Action action, bool pig_move) {
	if(children_nopigmove[action] == NULL) {
		const int role = t%2;
		int x=ps[role].x, y=ps[role].y, d=ps[role].d;
		if(action == A_FRONT) {
			switch(ps[role].d) {
			case D_NORTH: if(WALLS[y-1][x]) { y -= 1; } break;
			case D_SOUTH: if(WALLS[y+1][x]) { y += 1; } break;
			case D_WEST: if(WALLS[y][x-1]) { x -= 1; } break;
			case D_EAST: if(WALLS[y][x+1]) { x += 1; } break;
			}
		} else if(action == A_LEFT) {
			d = (d+4-1)%4;
		} else {
			d = (d+1)%4;
		}
		Node child(this, x, y, static_cast<Direction>(d), action);
		auto ret = children.emplace(make_pair(child.get_serialization(), child));
		children_nopigmove.at(action) = &ret.first->second;
	}

	if(!pig_move)
		return children_nopigmove.at(action);

	Node base(*children_nopigmove.at(action));
	// base.pig.x = something; modify
	auto ret = children.insert(make_pair(base.get_serialization(), base));
	// return object if it already existed
	return &ret.first->second;
}

bool Node::pig_trapped() const {
	static const int offsets[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
	for(size_t i=0; i<4; i++) {
		const int y = pig.y+offsets[i][1];
		const int x = pig.x+offsets[i][0];
		if(WALLS[y][x] &&
		   ((ps[0].x != x) || (ps[0].y != y)) &&
		   ((ps[1].x != x) || (ps[1].y != y)))
			return false;
	}
	return true;
}

bool Node::in_exit(int role) const {
	return (ps[role].x == 1 && ps[role].y == 3) ||
		(ps[role].x == 7 && ps[role].y == 3);
}

void Node::print() const {
	static char str[WALLS_W+1] = {'\0'};
	for(int y=0; y<WALLS_H; y++) {
		for(int x=0; x<WALLS_W; x++)
			str[x] = (WALLS[y][x] ? ' ' : '#');
		for(int i=0; i<2; i++) {
			if(y == ps[i].y)
				str[ps[i].x] = (ps[i].d == D_NORTH ? '^' :
								(ps[i].d == D_EAST ? '>' :
								(ps[i].d == D_SOUTH ? 'v' : '<')));
		}
		if(y==pig.y)
			str[pig.x] = '.';
		puts(str);
	}
	printf("t = %d, prev_action = %d, is_final = ", t, prev_action);
	cout << is_final << " pig_trapped = " << pig_trapped() << endl;
}

NodeSeri Node::get_serialization() const {
	const int serial = (pig.y
		| (pig.x << 4)
		| (ps[0].d << 8)
		| (ps[0].y << 10)
		| (ps[0].x << 14)
		| (ps[1].d << 18)
		| (ps[1].y << 20)
		| (ps[1].x << 24));
	return serial;
}
