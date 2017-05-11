#include "astar.hpp"

#include <set>
#include <utility>

// https://github.com/rhaps0dy/UPF-SWERC/blob/master/notebook2014/notebook.tex

void AStarSearch::search(int x0, int y0, Direction d0, int x1, int y1, Direction d1,
									int _P_x, int _P_y) {
	set<AStarNode, AStartNodeCompare> opened_nodes;
	set<AStarNode, AStartNodeCompare> closed_nodes;
	set< pair<int, AStarNode> > queue;

	AStarNode start_node(x0, y0, d0, x1, y1, d1, _P_x, _P_y);

	opened_nodes.insert(start_node);
	queue.insert(make_pair(start_node.get_f_cost(), start_node));

	while (!queue.empty()) {
		pair<int, AStarNode> p = *queue.begin();
		opened_nodes.erase(p.second);
		closed_nodes.insert(p.second);
		queue.erase(queue.begin());

		AStarNode& current_node = p.second;

		for (size_t i = 0; i < current_node.get_children().size(); ++i) {
			AStarNode& child_node = current_node.get_children()[i];

			if ( (opened_nodes.find(child_node) == opened_nodes.end() && closed_nodes.find(child_node) == closed_nodes.end() ) ) {
				opened_nodes.erase(child_node);
			}
		}
	}
}

AStarNode::AStarNode(const AStarNode *_parent, int x, int y, Direction d, Action _prev_a):
	Node(_parent, x, y, d, _prev_a) {}

AStarNode::AStarNode(int x0, int y0, Direction d0, int x1, int y1, Direction d1,
					 int _P_x, int _P_y):
	Node(x0, y0, d0, x1, y1, d1, _P_x, _P_y) {}

int AStarNode::get_heuristic_cost() const {
	return 0;
}

int AStarNode::get_f_cost() const {
	return g_cost + get_heuristic_cost();
}
