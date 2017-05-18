#include "path_cache.hpp"

#include <queue>


struct Position { int y,x,d; };

static void
bfs_explore(queue<Position> &&q,
			uint8_t cost[PEN_H][PEN_W][N_DIRECTIONS],
			uint8_t action[PEN_H][PEN_W][N_DIRECTIONS]) {
	while(!q.empty()) {
		Position p = q.front();
		q.pop();
		auto cur_cost = cost[p.y][p.x][p.d];
		if(p.x < 0 || p.x >= PEN_W) {
			// for the special case of exit_closest_cost
			cur_cost = 0;
		} else {
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
		}

		// The moving is also reversed
		switch(p.d) {
		case D_NORTH: p.y += 1; break;
		case D_SOUTH: p.y -= 1; break;
		case D_WEST: p.x += 1; break;
		case D_EAST: p.x -= 1; break;
		}
		if(0 < p.y && p.y < PEN_H && 0 < p.x && p.x < PEN_W &&
		   cost[p.y][p.x][p.d] < cur_cost) {
			cost[p.y][p.x][p.d] = cur_cost+1;
			action[p.y][p.x][p.d] = A_FRONT;
			q.push(p);
		}
	}
}

PathCache::PathCache() :
	pig_each_cost{}, pig_each_action{}, pig_exact_cost{}, pig_exact_action{},
	exit_closest_cost{}, exit_closest_action{}
{
	for(int pig_y=0; pig_y<PEN_H; pig_y++)
		for(int pig_x=0; pig_x<PEN_W; pig_x++)
			for(int d=0; d<N_DIRECTIONS; d++) {
				queue<Position> q;
				const int y = pig_y+PIG_AROUND_OFFSETS[d][1];
				const int x = pig_x+PIG_AROUND_OFFSETS[d][0];
				fill(&pig_each_cost[y][x][d][0][0][0],
					 &pig_each_cost[y][x][d][0][0][0]+PEN_H*PEN_W*N_DIRECTIONS,
					 static_cast<uint8_t>(0x7f));
				if(WALLS[y+1][x+2]) {
					for(int _d=0; _d<N_DIRECTIONS; _d++) {
						q.push({y, x, _d});
						pig_exact_cost[y][x][y][x][d] = 0;
					}
					bfs_explore(move(q), pig_each_cost[y][x][d],
								pig_each_action[y][x][d]);
				}
			}

	for(int pig_y=0; pig_y<PEN_H; pig_y++)
		for(int pig_x=0; pig_x<PEN_W; pig_x++) {
			queue<Position> q;
			fill(&pig_exact_cost[pig_y][pig_x][0][0][0],
				 &pig_exact_cost[pig_y][pig_x][0][0][0]+PEN_H*PEN_W*N_DIRECTIONS,
				 static_cast<uint8_t>(0x7f));
			for(int d=0; d<N_DIRECTIONS; d++) {
				q.push({pig_y, pig_x, d});
				pig_exact_cost[pig_y][pig_x][pig_y][pig_x][d] = 0;
			}
			bfs_explore(move(q), pig_exact_cost[pig_y][pig_x],
						pig_exact_action[pig_y][pig_x]);
		}

	queue<Position> q;
	fill(&exit_closest_cost[0][0][0],
		 &exit_closest_cost[0][0][0]+PEN_H*PEN_W*N_DIRECTIONS,
		 static_cast<uint8_t>(0x7f));
	for(int d=0; d<N_DIRECTIONS; d++) {
		// Accessing these illegal positions will not cause segfaults
		// since they're still within the array's memory region
		q.push({2, -1, d});
		q.push({2, 5, d});
	}
	bfs_explore(move(q), exit_closest_cost, exit_closest_action);
}

uint8_t *ffi_pig_optimal_costs() {
	return &(PathCache::get().pig_exact_cost[0][0][0][0][0]);
}
