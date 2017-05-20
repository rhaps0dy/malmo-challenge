#include "path_cache.hpp"

#include <queue>
#include <iostream>

struct Position { int y,x,d; };

static void
bfs_explore(queue<Position> &q,
			uint8_t cost[PEN_H][PEN_W][N_DIRECTIONS],
			uint8_t action[PEN_H][PEN_W][N_DIRECTIONS]) {
	while(!q.empty()) {
		Position p = q.front();
		q.pop();
		auto cur_cost = cost[p.y][p.x][p.d];
		// The turning directions are reversed
		const int d_right = (p.d+4-1)%4;
		if(cost[p.y][p.x][d_right] > cur_cost+1) {
			cost[p.y][p.x][d_right] = cur_cost+1;
			action[p.y][p.x][d_right] = A_RIGHT;
			q.push({p.y, p.x, d_right});
		}

		const int d_left = (p.d+1)%4;
		if(cost[p.y][p.x][d_left] > cur_cost+1) {
			cost[p.y][p.x][d_left] = cur_cost+1;
			action[p.y][p.x][d_left] = A_LEFT;
			q.push({p.y, p.x, d_left});
		}

		// The moving is also reversed
		switch(p.d) {
		case D_NORTH: p.y += 1; break;
		case D_SOUTH: p.y -= 1; break;
		case D_WEST: p.x += 1; break;
		case D_EAST: p.x -= 1; break;
		}
		if(0 <= p.y && p.y < PEN_H && 0 <= p.x && p.x < PEN_W &&
		   WALLS[p.y+1][p.x+1] && cost[p.y][p.x][p.d] > cur_cost) {
			cost[p.y][p.x][p.d] = cur_cost+1;
			action[p.y][p.x][p.d] = A_FRONT;
			q.push(p);
		}
	}
}

PathCache::PathCache() :
	location_cost{}, location_action{}, exit_closest_cost{},
	exit_closest_action{}
{
	fill(&location_cost[0][0][0][0][0], &location_cost[0][0][0][0][0] +
		 sizeof(location_cost)/sizeof(location_cost[0][0][0][0][0]),
		 static_cast<uint8_t>(0xff));
	for(int pig_y=0; pig_y<PEN_H; pig_y++)
		for(int pig_x=0; pig_x<PEN_W; pig_x++) {
			queue<Position> q;
			for(int d=0; d<N_DIRECTIONS; d++) {
				q.push({pig_y, pig_x, d});
				location_cost[pig_y][pig_x][pig_y][pig_x][d] = 0;
			}
			bfs_explore(q, location_cost[pig_y][pig_x],
						location_action[pig_y][pig_x]);
		}

	fill(&exit_closest_cost[0][0][0], &exit_closest_cost[0][0][0] +
		 sizeof(exit_closest_cost)/sizeof(exit_closest_cost[0][0][0]),
		 static_cast<uint8_t>(0xff));
	queue<Position> q;
	for(int d=0; d<N_DIRECTIONS; d++) {
		q.push({3, 1, d});
		q.push({3, 7, d});
		exit_closest_cost[3][1][d] = 0;
		exit_closest_cost[3][7][d] = 0;
	}

	bfs_explore(q, exit_closest_cost, exit_closest_action);

#ifndef NDEBUG
	for(int d=0; d<4; d++) {
		cout << "Direction " << d << ":\n";
		for(int pig_y=0; pig_y<PEN_H; pig_y++) {
			for(int pig_x=0; pig_x<PEN_W; pig_x++) {
				cout << (int)exit_closest_cost[pig_y][pig_x][d] << ", ";
			}
			cout << endl;
		}
	}
#endif
}

uint8_t *ffi_pig_optimal_costs() {
	return &(PathCache::get().location_cost[0][0][0][0][0]);
}
