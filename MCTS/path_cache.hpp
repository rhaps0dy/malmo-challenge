#pragma once

#include <cstdint>

#include "common.hpp"

using namespace std;

constexpr int PEN_H = WALLS_H-2;
constexpr int PEN_W = WALLS_W-2;

struct PathCache {
	static PathCache &get() {
		static PathCache instance;
		return instance;
	}

	uint8_t location_cost[PEN_H][PEN_W][PEN_H][PEN_W][N_DIRECTIONS];
	uint8_t location_action[PEN_H][PEN_W][PEN_H][PEN_W][N_DIRECTIONS];

	uint8_t exit_closest_cost[PEN_H][PEN_W][N_DIRECTIONS];
	uint8_t exit_closest_action[PEN_H][PEN_W][N_DIRECTIONS];

	PathCache();
private:
	PathCache(PathCache const&) = delete;
	void operator=(PathCache const&) = delete;
};

extern "C" {
	uint8_t *ffi_pig_optimal_costs();
}
