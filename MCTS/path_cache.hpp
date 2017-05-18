#pragma once

#include <cstdint>

#include "common.hpp"

using namespace std;

constexpr int PEN_H=5;
constexpr int PEN_W=5;

struct PathCache {
	static PathCache &get() {
		static PathCache instance;
		return instance;
	}

	uint8_t pig_each_cost[PEN_H][PEN_W][N_DIRECTIONS][PEN_H][PEN_W][N_DIRECTIONS];
	uint8_t pig_each_action[PEN_H][PEN_W][N_DIRECTIONS][PEN_H][PEN_W][N_DIRECTIONS];

	uint8_t pig_exact_cost[PEN_H][PEN_W][PEN_H][PEN_W][N_DIRECTIONS];
	uint8_t pig_exact_action[PEN_H][PEN_W][PEN_H][PEN_W][N_DIRECTIONS];

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
