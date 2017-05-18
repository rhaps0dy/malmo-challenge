#pragma once

typedef float Float;

// The map of the world; 1 are places where entities can move to, 0 are fences
typedef float Float;

constexpr int WALLS_W = 9;
constexpr int WALLS_H = 7;
const bool WALLS[WALLS_H][WALLS_W] = {
	{0,0,0,0,0,0,0,0,0},
	{0,0,1,1,1,1,1,0,0},
	{0,0,1,0,1,0,1,0,0},
	{0,1,1,1,1,1,1,1,0},
	{0,0,1,0,1,0,1,0,0},
	{0,0,1,1,1,1,1,0,0},
	{0,0,0,0,0,0,0,0,0}
};

enum Action {
	A_FRONT = 0,
	A_LEFT = 1,
	A_RIGHT = 2
};
constexpr int N_ACTIONS = 3;

// Time horizon after which the game ends.
// 25 player moves and 25 environment (other player + pig) moves
constexpr int MAX_T = 25*2-1;

enum Direction {
	D_NORTH = 0,
	D_EAST = 1,
	D_SOUTH = 2,
	D_WEST = 3
};
constexpr int N_DIRECTIONS = 4;
constexpr int PIG_AROUND_OFFSETS[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

struct Player {
	int x, y;
	Direction d;
	Player(int _x, int _y, Direction _d) : x(_x), y(_y), d(_d) {}
};
struct Pig {
	int x, y;
};

