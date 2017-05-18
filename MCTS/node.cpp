#include <cassert>
#include <iostream>
#include <utility>
#include <set>

#include "node.hpp"

const array<Action, N_ACTIONS> Node::actions = {{A_FRONT, A_LEFT, A_RIGHT}};

Node::Node(Node *_parent) :
	parent(_parent), prev_action(A_FRONT), is_final(false), value(0),
	t(parent->t), pig(parent->pig), ps(parent->ps), children()
{
	assert(t<=MAX_T);
	// prev_action and t are invalid, must call make_child
	check_if_final();
}

Node::Node(int x0, int y0, Direction d0, int x1, int y1, Direction d1,
		int _P_x, int _P_y) :
	parent(NULL), prev_action(A_FRONT), is_final(false), value(0), t(0),
	pig{_P_x, _P_y}, ps{{Player(x0, y0, d0), Player(x1, y1, d1)}}, children() {}


void Node::check_if_final() {
	const int role = t % 2;
	// role == 1 -> the environment has the final move
	if(role==1) {
		if(pig_trapped()) {
			value = 24-t/2;
			is_final = true;
		} else if(in_exit(0)) {
			value = 4-t/2;
			is_final = true;
		} else if(t>=MAX_T) {
			value = -t/2;
			is_final = true;
		}
	}
}

Node* Node::get_child(const Action action) {
	if(!children[action]) {
		children[action] = unique_ptr<Node>(new Node(this));
		children[action]->make_child(action);
	}
	return &*children[action];
}

bool Node::pig_trapped() const {
	for(size_t i=0; i<4; i++) {
		const int y = pig.y + PIG_AROUND_OFFSETS[i][1];
		const int x = pig.x + PIG_AROUND_OFFSETS[i][0];
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
	const int serial = (pig.y   +
		WALLS_H * (pig.x        +
		WALLS_W * (ps[0].d      +
		N_DIRECTIONS * (ps[0].y +
		WALLS_H * (ps[0].x      +
		WALLS_W * (ps[1].d      +
		N_DIRECTIONS * (ps[1].y +
		WALLS_H * (ps[1].x))))))));
	return serial;
}

void Node::make_child(const Action action, const bool pig_move) {
 	const int role = t%2;
	if(action == A_FRONT) {
		const int x = ps[role].x;
		const int y = ps[role].y;
		switch(ps[role].d) {
		case D_NORTH: if(WALLS[y-1][x]) { ps[role].y -= 1; } break;
		case D_SOUTH: if(WALLS[y+1][x]) { ps[role].y += 1; } break;
		case D_WEST: if(WALLS[y][x-1]) { ps[role].x -= 1; } break;
		case D_EAST: if(WALLS[y][x+1]) { ps[role].x += 1; } break;
		}
	} else if(action == A_LEFT) {
		ps[role].d = static_cast<Direction>((ps[role].d+4-1)%4);
	} else {
		ps[role].d = static_cast<Direction>((ps[role].d+1)%4);
	}
	t += 1;
	check_if_final();
}
