#pragma once

#include <vector>
#include <cassert>
#include <iostream>

#include "common.hpp"

using namespace std;

template<typename T>
class Node {
	friend int main(int argc, char *argv[]);
protected:
	const T *parent;
	Action prev_action;
	bool is_final;

	// State of the world
	int t;
	Pig pig;
	Player ps[2];
	vector<T> children;

public:
	Node(const T *_parent, int x, int y, Direction d, Action _prev_a) :
		parent(_parent), prev_action(_prev_a), is_final(false), t(parent->t+1),
		pig(parent->pig)
	{
		assert(_parent != static_cast<T*>(NULL));
		assert(t<=MAX_T);
		const int role = t % 2;
		if(role==1 && (pig_trapped() || in_exit(1) || in_exit(0) || t>=MAX_T))
			is_final = true;
		const int p_role = parent->t%2;
		ps[p_role].x = x;
		ps[p_role].y = y;
		ps[p_role].d = d;
		ps[role] = parent->ps[role];
	}

	Node(int x0, int y0, Direction d0, int x1, int y1, Direction d1,
			int _P_x, int _P_y) :
		parent(static_cast<T*>(NULL)), is_final(false), prev_action(A_FRONT), t(0)
	{
		ps[0].x = x0; ps[0].y = y0; ps[0].d = d0;
		ps[1].x = x1; ps[1].y = y1; ps[1].d = d1;
		pig.x = _P_x; pig.y = _P_y;
	}
	virtual ~Node() {};

	vector<T> &get_children()
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
		children.push_back(T(dynamic_cast<T*>(this), ps[role].x, ps[role].y,
							 static_cast<Direction>((ps[role].d+4-1)%4), A_LEFT));
		children.push_back(T(dynamic_cast<T*>(this), ps[role].x, ps[role].y,
							 static_cast<Direction>((ps[role].d+1)%4), A_RIGHT));
		dx += ps[role].x;
		dy += ps[role].y;
		if(WALLS[dy][dx]) {
			children.push_back(T(dynamic_cast<T*>(this), dx, dy, ps[role].d, A_FRONT));
		}
		return children;
	}

	bool pig_trapped() const {
		for(int dx=-1; dx<2; dx+=2)
			for(int dy=-1; dy<2; dy+=2) {
				const int y = pig.y+dy;
				const int x = pig.x+dx;
				if(WALLS[y][x] &&
				((ps[0].x != x) || (ps[0].y != y)) &&
				((ps[1].x != x) || (ps[1].y != y)))
					return false;
			}
		return true;
	}

	bool in_exit(int role) const {
		return (ps[role].x == 1 && ps[role].y == 3) ||
			(ps[role].x == 7 && ps[role].y == 3);
	}

	void print() const {
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
		cout << is_final << ' ' << pig_trapped() << endl;
	}
};
