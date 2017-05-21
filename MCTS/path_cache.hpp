#pragma once

#include <cstdint>
#include <cassert>

#include "common.hpp"
#include "node.hpp"

using namespace std;

constexpr int PEN_H = WALLS_H;
constexpr int PEN_W = WALLS_W;

enum Objective {
	OBJECTIVE_EXIT,
	OBJECTIVE_PIG,
	OBJECTIVE_CORNER_PIG
};

class PathCache {
public:
	static PathCache &get() {
		static PathCache instance;
		return instance;
	}

	template<const int role, const Objective obj> static inline Action
	get_action(const Node &n) {
		assert(n.ps[role].y >= 0 && n.ps[role].y < PEN_H);
		assert(n.ps[role].x >= 0 && n.ps[role].x < PEN_W);
		assert(n.ps[role].d >= 0 && n.ps[role].d < N_DIRECTIONS);
		if(obj == OBJECTIVE_EXIT) {
			return static_cast<Action>(get().exit_closest_action
									   [n.ps[role].y][n.ps[role].x]
									   [n.ps[role].d]);
		} else {
			assert(n.pig.y >= 0 && n.pig.y < PEN_H);
			assert(n.pig.x >= 0 && n.pig.x < PEN_W);
			return static_cast<Action>(get().location_action
									   [n.pig.y][n.pig.x]
									   [n.ps[role].y][n.ps[role].x]
									   [n.ps[role].d]);
		}
	}
	template<const int role, const Objective obj> static inline uint8_t
	get_cost(const Node &n) {
		assert(n.ps[role].y >= 0 && n.ps[role].y < PEN_H);
		assert(n.ps[role].x >= 0 && n.ps[role].x < PEN_W);
		assert(n.ps[role].d >= 0 && n.ps[role].d < N_DIRECTIONS);
		if(obj == OBJECTIVE_EXIT) {
			return get().exit_closest_cost
				[n.ps[role].y][n.ps[role].x][n.ps[role].d];
		} else {
			assert(n.pig.y >= 0 && n.pig.y < PEN_H);
			assert(n.pig.x >= 0 && n.pig.x < PEN_W);
			return get().location_cost[n.pig.y][n.pig.x]
									  [n.ps[role].y][n.ps[role].x]
									  [n.ps[role].d];
		}
	}

	// How much does it cost and what action to take to go from [2,3; 4] to [0,1]
	uint8_t location_cost[PEN_H][PEN_W][PEN_H][PEN_W][N_DIRECTIONS];
	uint8_t location_action[PEN_H][PEN_W][PEN_H][PEN_W][N_DIRECTIONS];

	// How to go to the closest exit and how much does it cost, from [0,1; 2]
	uint8_t exit_closest_cost[PEN_H][PEN_W][N_DIRECTIONS];
	uint8_t exit_closest_action[PEN_H][PEN_W][N_DIRECTIONS];

	// The location the player should pursue to corner the pig, if the pig is in
	// [0,1] and the other player is in [2,3], looking to [4]
	uint8_t pig_corner_cost[PEN_H][PEN_W][PEN_H][PEN_W][N_DIRECTIONS][PEN_H][PEN_W][N_DIRECTIONS];
	uint8_t pig_corner_action[PEN_H][PEN_W][PEN_H][PEN_W][N_DIRECTIONS][PEN_H][PEN_W][N_DIRECTIONS];

private:
	PathCache();

	PathCache(PathCache const&) = delete;
	void operator=(PathCache const&) = delete;
};

extern "C" {
	uint8_t *ffi_pig_optimal_costs();
}
