#pragma once
#include <cstdlib>

#include "node.hpp"

typedef int (*AStarHeuristic)(const Node &);
typedef bool (*AStarGoal)(const Node &);

class AStarSearch {
public:
	void search(int x0, int y0, Direction d0, int x1, int y1, Direction d1,
		int _P_x, int _P_y);
};

struct AStarNodeCompare {
	bool operator() (const AStarNode& lhs, const AStarNode& rhs) const {
		return lhs.get_serialization() < rhs.get_serialization();
	}
};

bool exit_goal(const Node &n) {
	return n.in_exit(0);
}
int exit_heuristic(const Node &n) {
	// Manhattan distance of p0 to closest exit
	const int y_dist = abs(n.ps[0].y - 3);
	const int x_dist = min(abs(n.ps[0].x - 1), abs(n.ps[0].x - 7));
	return (y_dist + x_dist) * 2;
}
