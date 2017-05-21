#include "path_cache.hpp"

#include <deque>
#include <iostream>
#include <utility>
#include <vector>

#include <sys/types.h>
#include <unistd.h>

struct Position { int y,x,d; };

static void
bfs_explore(deque<Position> &q,
			uint8_t cost[PEN_H][PEN_W][N_DIRECTIONS],
			uint8_t action[PEN_H][PEN_W][N_DIRECTIONS]) {
	for(auto p: q)
		cost[p.y][p.x][p.d] = 0;
	while(!q.empty()) {
		Position p = q.front();
		q.pop_front();
		auto cur_cost = cost[p.y][p.x][p.d];
		// The turning directions are reversed
		const int d_right = (p.d+4-1)%4;
		if(cost[p.y][p.x][d_right] > cur_cost+1) {
			cost[p.y][p.x][d_right] = cur_cost+1;
			action[p.y][p.x][d_right] = A_RIGHT;
			q.push_back({p.y, p.x, d_right});
		}

		const int d_left = (p.d+1)%4;
		if(cost[p.y][p.x][d_left] > cur_cost+1) {
			cost[p.y][p.x][d_left] = cur_cost+1;
			action[p.y][p.x][d_left] = A_LEFT;
			q.push_back({p.y, p.x, d_left});
		}

		// The moving is also reversed
		switch(p.d) {
		case D_NORTH: p.y += 1; break;
		case D_SOUTH: p.y -= 1; break;
		case D_WEST: p.x += 1; break;
		case D_EAST: p.x -= 1; break;
		}
		if(0 <= p.y && p.y < PEN_H && 0 <= p.x && p.x < PEN_W &&
		   WALLS[p.y][p.x] && cost[p.y][p.x][p.d] > cur_cost) {
			cost[p.y][p.x][p.d] = cur_cost+1;
			action[p.y][p.x][p.d] = A_FRONT;
			q.push_back(p);
		}
	}
}

static void
print_array(uint8_t arr[PEN_H][PEN_W][N_DIRECTIONS]) {
	for(int d=0; d<N_DIRECTIONS; d++) {
		cout << "Direction " << d << ":\n";
		for(int y=0; y<PEN_H; y++) {
			for(int x=0; x<PEN_W; x++)
				cout << static_cast<int>(arr[y][x][d]) << " ";
			cout << endl;
		}
		cout << endl;
	}
}

#define FOR(var, limit) for(int var=0; var<limit; var++)

static uint8_t
get_min_directions(vector<Direction> &min_directions, uint8_t loc_orient_cost[PEN_H][PEN_W][N_DIRECTIONS][PEN_H][PEN_W][N_DIRECTIONS],
				   int pig_y, int pig_x, int p_y, int p_x, int p_d) {
	uint8_t min_direction_cost=0xff;
	min_directions.clear();
	FOR(pig_d, N_DIRECTIONS) {
		if(WALLS[pig_y+PIG_AROUND_OFFSETS[pig_d][0]]
		   [pig_x+PIG_AROUND_OFFSETS[pig_d][1]]) {
			if(loc_orient_cost[pig_y][pig_x][pig_d][p_y][p_x][p_d] < min_direction_cost) {
				min_direction_cost = loc_orient_cost[pig_y][pig_x][pig_d][p_y][p_x][p_d];
				min_directions.clear();
				min_directions.push_back(static_cast<Direction>(pig_d));
			} else if(loc_orient_cost[pig_y][pig_x][pig_d][p_y][p_x][p_d] == min_direction_cost) {
				min_directions.push_back(static_cast<Direction>(pig_d));
			}
		}
	}
	return min_direction_cost;
}

PathCache::PathCache() :
	location_cost{}, location_action{}, exit_closest_cost{},
	exit_closest_action{}
{
	// The cost and action to go to the square adjacent in direction (2) to a
	// location ([1], [0]), facing direction ([2]+2) % 4.
	// An intermediate result for location_{cost,action} and pig_corner_location
	uint8_t loc_orient_cost[PEN_H][PEN_W][N_DIRECTIONS][PEN_H][PEN_W][N_DIRECTIONS];
	uint8_t loc_orient_action[PEN_H][PEN_W][N_DIRECTIONS][PEN_H][PEN_W][N_DIRECTIONS] = {};

	fill(&loc_orient_cost[0][0][0][0][0][0], &loc_orient_cost[0][0][0][0][0][0] +
		 sizeof(loc_orient_cost)/sizeof(loc_orient_cost[0][0][0][0][0][0]),
		 static_cast<uint8_t>(0xfe)); // 0xfe because we will sum 1 later
	FOR(pig_y, PEN_H) {
		FOR(pig_x, PEN_W) {
			if(!WALLS[pig_y][pig_x])
				continue;
			deque<Position> q;
			FOR(d, N_DIRECTIONS) {
				const int y = pig_y+PIG_AROUND_OFFSETS[d][0];
				const int x = pig_x+PIG_AROUND_OFFSETS[d][1];
				if(!WALLS[y][x])
					continue;
				const int _d = (d+2)%4;
				q.push_back({y, x, _d});
				bfs_explore(q, loc_orient_cost[pig_y][pig_x][d],
							loc_orient_action[pig_y][pig_x][d]);
			}
		}
	}

	// The cost and action of going to a location
	fill(&location_cost[0][0][0][0][0], &location_cost[0][0][0][0][0] +
		 sizeof(location_cost)/sizeof(location_cost[0][0][0][0][0]),
		 static_cast<uint8_t>(0xff));
	FOR(pig_y, PEN_H) {
		FOR(pig_x, PEN_W) {
			FOR(pig_d, N_DIRECTIONS) {
				FOR(y, PEN_H) {
					FOR(x, PEN_W) {
						FOR(d, N_DIRECTIONS) {
							const auto LOC = loc_orient_cost[pig_y][pig_x][pig_d][y][x][d]+1;
							if(LOC < location_cost[pig_y][pig_x][y][x][d]) {
								location_cost[pig_y][pig_x][y][x][d] = LOC;
								location_action[pig_y][pig_x][y][x][d] =
									loc_orient_action[pig_y][pig_x][pig_d][y][x][d];
								assert(loc_orient_action[pig_y][pig_x][pig_d][y][x][d] < N_ACTIONS);
							}
	}}}}}}

	fill(&pig_corner_cost[0][0][0][0][0][0][0][0], &pig_corner_cost[0][0][0][0][0][0][0][0] +
		 sizeof(pig_corner_cost)/sizeof(pig_corner_cost[0][0][0][0][0][0][0][0]),
		 static_cast<uint8_t>(0xff));
	FOR(pig_y, PEN_H) {
		FOR(pig_x, PEN_W) {
			if(!WALLS[pig_y][pig_x])
				continue;
			FOR(p1_y, PEN_H) {
				FOR(p1_x, PEN_W) {
					if(!WALLS[p1_y][p1_x])
						continue;
					FOR(p1_d, N_DIRECTIONS) {
						// First, we determine which are the square/s adjacent
						// to the pig that have no wall and is/are closest to
						// the player we do NOT control
						static vector<Direction> min_directions_p1;
						get_min_directions(min_directions_p1, loc_orient_cost, pig_y, pig_x, p1_y, p1_x, p1_d);
						// There should be at least 1 of them.
						assert(min_directions_p1.size() >= 1);
						assert(min_directions_p1.size() <= 2 || (pig_y==p1_y && pig_x==p1_x));
						// It should be sorted
						for(size_t i=1; i<min_directions_p1.size(); i++)
							assert(min_directions_p1[i] > min_directions_p1[i-1]);

						// Now calculate the directions which are both: not the
						// closest to p1 and accessible
						static vector<Direction> feasible_directions_p0;
						feasible_directions_p0.clear();
						size_t i=0;
						FOR(d, N_DIRECTIONS) {
							if(i<min_directions_p1.size() && d == min_directions_p1[i]) {
								i++;
							} else if(WALLS[pig_y+PIG_AROUND_OFFSETS[d][0]][pig_x+PIG_AROUND_OFFSETS[d][1]]) {
								feasible_directions_p0.push_back(static_cast<Direction>(d));
							}
						}

						if(feasible_directions_p0.size() == 0) {
							// If no directions are so feasible, just go to the pig.
							FOR(p0_y, PEN_H) {
								FOR(p0_x, PEN_W) {
									FOR(p0_d, N_DIRECTIONS) {
										pig_corner_cost[pig_y][pig_x][p1_y][p1_x][p1_d][p0_y][p0_x][p0_d] =
											location_cost[pig_y][pig_x][p0_y][p0_x][p0_d];
										pig_corner_action[pig_y][pig_x][p1_y][p1_x][p1_d][p0_y][p0_x][p0_d] =
											location_action[pig_y][pig_x][p0_y][p0_x][p0_d];
										assert(location_action[pig_y][pig_x][p0_y][p0_x][p0_d] < N_ACTIONS);
									}
								}
							}
						} else {
							// otherwise take all the feasible ones
							deque<Position> q;
							for(Direction d: feasible_directions_p0) {
								FOR(p0_d, N_DIRECTIONS) {
									q.push_back({pig_y+PIG_AROUND_OFFSETS[d][0],
											pig_x+PIG_AROUND_OFFSETS[d][1], p0_d});
								}
							}
							bfs_explore(q,
										pig_corner_cost[pig_y][pig_x][p1_y][p1_x][p1_d],
										pig_corner_action[pig_y][pig_x][p1_y][p1_x][p1_d]);
						}
	}}}}}

	// Cost and action to go to the closest exit
	fill(&exit_closest_cost[0][0][0], &exit_closest_cost[0][0][0] +
		 sizeof(exit_closest_cost)/sizeof(exit_closest_cost[0][0][0]),
		 static_cast<uint8_t>(0xff));
	deque<Position> q;
	for(int d=0; d<N_DIRECTIONS; d++) {
		q.push_back({3, 1, d});
		q.push_back({3, 7, d});
	}

	bfs_explore(q, exit_closest_cost, exit_closest_action);

#ifndef NDEBUG
//	print_array(pig_corner_cost[1][6][3][5][D_EAST]);
//	print_array(exit_closest_cost);
//	print_array(location_cost[1][6]);
//	print_array(location_action[1][6]);
//	print_array(pig_corner_cost[5][3][3][3][2]);
//	print_array(pig_corner_action[5][3][3][3][2]);
//	print_array(pig_corner_action[3][7][4][6][1]);
	cout << "Current PID:" << ::getpid() << endl;
#endif
}

uint8_t *ffi_pig_optimal_costs() {
	return &(PathCache::get().location_cost[0][0][0][0][0]);
}
