#include <cassert>
#include <iostream>
#include <cstdio>

#include "node.hpp"

Node::Node(Node &_parent, int x, int y, Direction d, Action _prev_a) :
	parent(_parent), prev_action(_prev_a), is_final(false), value_sum(0.0),
	n_visits(0), t(parent.t+1), pig(parent.pig)
{
	assert(t<=MAX_T);
	const int role = t % 2;
	const int p_role = parent.t%2;
	ps[p_role].x = x;
	ps[p_role].y = y;
	ps[p_role].d = d;
	ps[role] = parent.ps[role];

	// role == 1 -> the environment has the final move
	if(role==1 && (pig_trapped() || in_exit(1) || in_exit(0) || t>=MAX_T))
		is_final = true;
}

Node::Node(int x0, int y0, Direction d0, int x1, int y1, Direction d1,
		int _P_x, int _P_y) :
	parent(*this), prev_action(A_FRONT), is_final(false), value_sum(0.0),
	n_visits(0), t(0)
{
	ps[0].x = x0; ps[0].y = y0; ps[0].d = d0;
	ps[1].x = x1; ps[1].y = y1; ps[1].d = d1;
	pig.x = _P_x; pig.y = _P_y;
}

vector<Node> &Node::get_children()
{
	assert(!is_final);
	if(children.size() != 0)
		return children;

	const int role = t%2;
	int dx=0, dy=0;
	switch(ps[role].d) {
	case D_NORTH:
		dy=-1;
		break;
	case D_SOUTH:
		dy=1;
		break;
	case D_WEST:
		dx=-1;
		break;
	case D_EAST:
		dx=1;
		break;
	}
	children.reserve(N_ACTIONS);
	children.emplace_back(*this, ps[role].x, ps[role].y,
							static_cast<Direction>((ps[role].d+4-1)%4), A_LEFT);
	children.emplace_back(*this, ps[role].x, ps[role].y,
							static_cast<Direction>((ps[role].d+1)%4), A_RIGHT);
	dx += ps[role].x;
	dy += ps[role].y;
	if(WALLS[dy][dx]) {
		children.emplace_back(*this, dx, dy, ps[role].d, A_FRONT);
	}
	return children;
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

int Node::get_serialization() const {
	int serial = 0;
	serial |= pig.y;
	serial |= pig.x << 4;
	serial |= ps[1].d << 8;
	serial |= ps[1].y << 10;
	serial |= ps[1].x << 14;
	serial |= ps[0].d << 18;
	serial |= ps[0].y << 20;
	serial |= ps[0].x << 24;
	return serial;
}
