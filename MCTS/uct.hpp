#pragma once

#include <vector>
#include <utility>

#include "common.hpp"
#include "node.hpp"

using namespace std;

Float uct_value(const Node &n, const Float c);
Node &simulate_path(Node &current, Float constant=1.);
