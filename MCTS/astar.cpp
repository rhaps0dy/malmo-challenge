#include "astar.hpp"

#include <set>
#include <utility>

using namespace std;

static int get_f_cost(const Node &n, AStarHeuristic h) {
	return n.t + h(n);
}

// https://github.com/rhaps0dy/UPF-SWERC/blob/master/notebook2014/notebook.tex

void AStarSearch::search(int x0, int y0, Direction d0, int x1, int y1, Direction d1,
						 int _P_x, int _P_y, AStarGoal goal, AStarHeuristic heuristic) {
	set<Node, AStarNodeCompare> opened_nodes;
	set<Node, AStarNodeCompare> closed_nodes;
	set< pair<int, Node> > queue;

	Node start_node(x0, y0, d0, x1, y1, d1, _P_x, _P_y);

	opened_nodes.insert(start_node);
	queue.insert(make_pair(get_f_cost(start_node, heuristic), start_node));

	while (!queue.empty()) {
		pair<int, Node> p = *queue.begin();
		opened_nodes.erase(p.second);
		closed_nodes.insert(p.second);
		queue.erase(queue.begin());

		Node& current_node = p.second;

		for (size_t i = 0; i < current_node.get_children().size(); ++i) {
			Node& child_node = current_node.get_children()[i];

			if ( (opened_nodes.find(child_node) == opened_nodes.end() && closed_nodes.find(child_node) == closed_nodes.end() ) ) {
				opened_nodes.erase(child_node);
			}
		}
	}
}
