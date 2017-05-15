#pragma once
#include <cstdlib>
#include <iostream>
#include <set>
#include <utility>
#include <unordered_map>
#include <cassert>

#include "node.hpp"

typedef int (*AStarFCost)(const Node &);
typedef bool (*AStarGoal)(const Node &);


template<int role>
bool exit_goal(const Node &n) {
	return n.in_exit(role);
}
template<int role>
int exit_f_cost(const Node &n) {
	// Manhattan distance of ps[role] to closest exit + time for that role
	const int y_dist = abs(n.ps[role].y - 3);
	const int x_dist = min(abs(n.ps[role].x - 1), abs(n.ps[role].x - 7));
	return (y_dist + x_dist) + n.t/2;
}

template<int role> static
int manhattan_pig(const Node &n) {
	return abs(n.ps[role].x-n.pig.x)+abs(n.ps[role].y-n.pig.y);
}

template<int role>
bool pig_position_goal(const Node &n) {
	return manhattan_pig<role>(n) == 1;
}
template<int role>
int pig_position_f_cost(const Node &n) {
	// Manhattan distance of ps[role] to pig
	return max(0, manhattan_pig<role>(n) - 1) + n.t/2;
}

bool pig_optimal_goal(const Node &n) {
	return n.pig_trapped();
}
int pig_optimal_f_cost(const Node &n) {
	// minimum Manhattan distance of players to surrounding pig
	return max(0, manhattan_pig<0>(n)+manhattan_pig<1>(n)-2) + n.t;
}


template<AStarFCost f_cost, AStarGoal goal, bool two_roles>
Node &astar_search(Node &root) {
	set<NodeSeri> closed;
	set< pair<int, Node*> > queue;

	queue.insert(make_pair(f_cost(root), &root));
	while (!queue.empty()) {
		auto p = queue.begin();
		Node &n = *p->second;
		if(goal(n))
			return n;

		queue.erase(p);
		const NodeSeri p_seri = n.get_serialization();
		if(closed.find(p_seri) != closed.end())
			continue;
		closed.insert(p_seri);

		for(const Action a: Node::actions) {
			auto child = n.get_child(a);
//			if(two_roles) {
//				if(closed.find(child.get_serialization()) == closed.end())
//					queue.insert(make_pair(f_cost(child), &child));
//			} else {
				auto child2 = child.get_child(A_LEFT);
				if((long)(&child2) > 0x0000000200200140) {
					;
				}
				if(closed.find(child2.get_serialization()) == closed.end())
					queue.insert(make_pair(f_cost(child2), &child2));
//			}
		}
		cout << "Queue size: " << queue.size() << endl;
	}
	assert(false); // No path from root to a goal
	return root;
}