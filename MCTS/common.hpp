#pragma once

typedef float Float;

// The map of the world; 1 are places where entities can move to, 0 are fences
typedef float Float;

const bool WALLS[7][9] = {
	{0,0,0,0,0,0,0,0,0},
	{0,0,1,1,1,1,1,0,0},
	{0,0,1,0,1,0,1,0,0},
	{0,1,1,1,1,1,1,1,0},
	{0,0,1,0,1,0,1,0,0},
	{0,0,1,1,1,1,1,0,0},
	{0,0,0,0,0,0,0,0,0}
};

enum Action {
	A_LEFT = 0,
	A_RIGHT = 1,
	A_FRONT = 2
};
constexpr int N_ACTIONS = 3;

// Time horizon after which the game ends.
// 25 player moves and 25 environment (other player + pig) moves
constexpr int MAX_T = 25*2;

enum Direction {
	D_NORTH = 0,
	D_EAST = 1,
	D_SOUTH = 2,
	D_WEST = 3
};

struct Player {
	int x, y;
	Direction d;
};
struct Pig {
	int x, y;
};
