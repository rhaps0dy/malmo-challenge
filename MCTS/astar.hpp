#pragma once

#include "node.hpp"

class AStarSearch {
public:
	void search(int x0, int y0, Direction d0, int x1, int y1, Direction d1,
		int _P_x, int _P_y);
};

struct AStartNodeCompare {
	bool operator() (const AStarNode& lhs, const AStarNode& rhs) const {
		return lhs.get_serialization() < rhs.get_serialization();
	}
}

class AStarNode : public Node<AStarNode> {
protected:
	int g_cost; // accumulated cost

public:
	AStarNode(const AStarNode *_parent, int x, int y, Direction d, Action _prev_a);
	AStarNode(int x0, int y0, Direction d0, int x1, int y1, Direction d1,
			int _P_x, int _P_y);
	int get_heuristic_cost() const;
	int get_f_cost() const;
};
