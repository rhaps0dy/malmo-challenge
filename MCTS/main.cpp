#include <vector>
#include <cmath>
#include <iostream>
#include <utility>
#include <random>

#include "common.hpp"
#include "uct.hpp"

// Learn the strategy of the pig
// Learn the possible strategy/s of the other agent

int main(int argc, char *argv[])
{
	UCTNode root(2,1,D_NORTH, 5,1,D_SOUTH, 5, 3);
	const UCTNode *fnode = root.simulate_path();
	while(fnode != NULL) {
		fnode->print();
		fnode = fnode->parent;
	}
	return 0;
}
