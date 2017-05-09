#include "common.hpp"
#include "uct.hpp"

// Learn the strategy of the pig
// Learn the possible strategy/s of the other agent

int main(int argc, char *argv[])
{
	UCTNode root(0, 0, D_NORTH, 0, 1, D_EAST, 0, 2);
	/*
	auto acts = n.available_actions();
	for(auto it=acts.begin(); it!=acts.end(); it++) {
		cout << *it << ' ';
	}
	cout << endl;*/
	return 0;
}
