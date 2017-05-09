#include <vector>
#include <cmath>
#include <cassert>
#include <iostream>
#include <utility>

// Learn the strategy of the pig
// Learn the possible strategy/s of the other agent

using namespace std;

typedef float Float;
const bool WALLS[7][7] = {
	{0,0,0,0,0,0,0},
	{0,1,1,1,1,1,0},
	{0,1,0,1,0,1,0},
	{1,1,1,1,1,1,1},
	{0,1,0,1,0,1,0},
	{0,1,1,1,1,1,0},
	{0,0,0,0,0,0,0}
};
const int N_ACTIONS = 3;
const int MAX_T = 25;

enum Action {
	A_LEFT = 0,
	A_RIGHT = 1,
	A_FRONT = 2
};

enum Direction {
	D_NORTH,
	D_EAST,
	D_SOUTH,
	D_WEST
};

struct Player {
	int x, y;
	Direction d;
};

struct Pig {
	int x, y;
};

class Strategy {
	virtual Action act(Player p1, Player p2, int t, Pig pig) = 0;
};

class Left : Strategy {
	virtual Action act(Player p1, Player p2, int t, Pig pig) { return A_LEFT; }
};
class Right : Strategy {
	virtual Action act(Player p1, Player p2, int t, Pig pig) { return A_RIGHT; }
};
class Front : Strategy {
	virtual Action act(Player p1, Player p2, int t, Pig pig) { return A_FRONT; }
};

struct Node {
	const Node &parent;
	Float value_sum;
	int n_visits;
	bool is_final;
	Action prev_action;
	bool visited_all_children;

	// State of the world
	int t;
	Pig pig
	Player ps[2];
	static vector<pair<Float, Strategy> > p1_strats;

protected:
	vector<Node> children;

public:

	Node(const Node &_parent, int x, int y, Direction d, Action _prev_a) :
		parent(_parent), value_sum(0.0), n_visits(0), is_final(false),
		prev_action(_prev_a), visited_all_children(false), t(parent.t+1), pig(parent.pig) {
		const int role=t%2;
		if(role==1 && ((x == 0) && (y == 3) || (x == 6) && (y == 3) || t >= MAX_T)) {
			is_final = true;
		}
	}

	// Constructor for the root
	Node(int x0, int y0, Direction d0, int x1, int y1, Direction d1, int _P_x, int _P_y) :
		parent(*this), value_sum(0.0), n_visits(0), is_final(false),
		prev_action(A_FRONT), visited_all_children(false), t(0) {
		ps[0].x = x0; ps[0].y = y0; ps[0].d = d0;
		ps[1].x = x1; ps[1].y = y1; ps[1].d = d1;
		pig.x = _P_x; pig.y = _P_y;

	}

	const vector<Node> &get_children() {
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
		children.emplace_back(*this, ps[role].x, ps[role].y, (ps[role].d+4-1)%4, A_LEFT, t+1);
		children.emplace_back(*this, ps[role].x, ps[role].y, (ps[role].d+1)%4, A_RIGHT, t+1);
		dx += ps[role].x;
		dy += ps[role].y;
		if(WALLS[dy][dx]) {
			children.emplace_back(*this, dx, dy, ps[role].d, A_FRONT, t+1);
		}
		return children;
	}

	const Float value(const Float c) {
		if(is_final) {
			Float v = -(t/2);
			if(ps[0].x == 
			return v;
		}
		return value_sum/n_visits + c*sqrt(2.*log(parent.n_visits)/n_visits);
	}

	Node &simulate_path(
};


int main(int argc, char *argv[])
{
	Node root;
	auto acts = n.available_actions();
	for(auto it=acts.begin(); it!=acts.end(); it++) {
		cout << *it << ' ';
	}
	cout << endl;
	return 0;
}
