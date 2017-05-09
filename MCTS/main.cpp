#include <vector>
#include <cmath>
#include <cassert>
#include <iostream>
#include <utility>
#include <random>

// Learn the strategy of the pig
// Learn the possible strategy/s of the other agent

using namespace std;

// The map of the world; 1 are places where entities can move to, 0 are fences
typedef float Float;
const bool WALLS[7][9] = {
	{0,0,0,0,0,0,0,0,0},
	{0,0,1,1,1,1,1,0,0},
	{0,0,1,0,1,0,1,0,0},
	{0,1,1,1,1,1,1,1,0},
	{0,0,1,0,1,0,1,0,0},
	{0,0,1,1,1,1,1,0,0},
	{0,0,0,0,0,0,0,0,0}
};

enum Action {
	A_LEFT = 0,
	A_RIGHT = 1,
	A_FRONT = 2
};
constexpr int N_ACTIONS = 3;

// Time horizon after which the game ends.
// 25 player moves and 25 environment (other player + pig) moves
constexpr int MAX_T = 25*2;

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
	virtual Action act(Player p1, Player p2, int t, Pig pig) const = 0;
};

class Left : Strategy {
	virtual Action act(Player p1, Player p2, int t, Pig pig) const { return A_LEFT; }
};
class Right : Strategy {
	virtual Action act(Player p1, Player p2, int t, Pig pig) const { return A_RIGHT; }
};
class Front : Strategy {
	virtual Action act(Player p1, Player p2, int t, Pig pig) const { return A_FRONT; }
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
	Pig pig;
	Player ps[2];

protected:
	vector<Node> children;

	static default_random_engine &generator;
	static uniform_real_distribution<Float> uniform;
	static vector<pair<Float, Strategy> > p1_strats;
public:

	Node(const Node &_parent, int x, int y, Direction d, Action _prev_a) :
		parent(_parent), value_sum(0.0), n_visits(0), is_final(false),
		prev_action(_prev_a), visited_all_children(false), t(parent.t+1), pig(parent.pig) {
		if(pig_trapped() || in_exit(1)) // implicit "&& role==1"
			is_final = true;
	}

	// Constructor for the root
	Node(int x0, int y0, Direction d0, int x1, int y1, Direction d1,
		 int _P_x, int _P_y, default_random_engine &gen) :
		parent(*this), value_sum(0.0), n_visits(0), is_final(false),
		prev_action(A_FRONT), visited_all_children(false), t(0) {
		ps[0].x = x0; ps[0].y = y0; ps[0].d = d0;
		ps[1].x = x1; ps[1].y = y1; ps[1].d = d1;
		pig.x = _P_x; pig.y = _P_y;

		generator = gen;
		uniform = uniform_real_distribution<Float>(0.0, 1.0);
	}

	static const Strategy &random_choose_strat() {
		Float f = uniform(generator);
		Float sum = 0.0;
		for(size_t i=0;;i++) {
			sum += p1_strats[i].first;
			if(sum > f)
				return p1_strats[i].second;
		}
		// probabilities should sum to 1 so this shouldn't be reached
		assert(false);
	}

	vector<Node> &get_children() {
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

	bool pig_trapped() const {
		for(int dx=-1; dx<2; dx+=2)
			for(int dy=-1; dy<2; dy+=2) {
				const int y = pig.y+dy;
				const int x = pig.x+dx;
				if(!WALLS[y][x] &&
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

	const Float value(const Float c) const {
		if(is_final) {
			Float v = -(t/2);
			if(pig_trapped()) return v+25.;
			if(in_exit(0)) return v+5.;
			return v;
		}
		return value_sum/n_visits + c*sqrt(2.*log(parent.n_visits)/n_visits);
	}
};

Node &simulate_path(Node &_current, Float constant=1.) {
	const Strategy &strat = Node::random_choose_strat();
	Node *current = &_current;
	while(!current->is_final) {
		const int role = current->t%2;
		if(role == 0) {
			size_t i=0, best_i=0;
			Float max_val = current->get_children()[0].value(constant);
			for(i=0; i<current->get_children().size(); i++) {
				auto val = current->get_children()[i].value(constant);
				if(val > max_val) {
					max_val = val;
					best_i = i;
				}
			}
			current = &current->get_children()[i];
		} else {
			Action a = strat.act(current->p[0], current->p[1], current->t,
								 current->pig);
			current = &current->get_children()[(int)a];
		}
	}
	return *current;
}


int main(int argc, char *argv[])
{
	default_random_engine generator;
	Node root(2,1,D_NORTH, 5,1,D_SOUTH, 5, 3, generator);
	auto acts = n.available_actions();
	for(auto it=acts.begin(); it!=acts.end(); it++) {
		cout << *it << ' ';
	}
	cout << endl;
	return 0;
}
