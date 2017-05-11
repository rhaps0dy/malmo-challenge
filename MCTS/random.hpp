#pragma once

#include <random>
#include <chrono>

#include "common.hpp"

using namespace std;

class Random {
	Random(Random const&) = delete;
	void operator=(Random const&) = delete;
	Random() :
		generator(chrono::system_clock::now().time_since_epoch().count()) {}
	default_random_engine generator;

public:
	static Random &get() {
		static Random instance;
		return instance;
	}
	template<typename T>
	static T uniform(void) {
		static uniform_real_distribution<T> d(0.0, 1.0);
		return d(get().generator);
	}
	template<typename T>
	static T uniform_int(T max_val) {
		uniform_int_distribution<T> d(0, max_val);
		return d(get().generator);
	}
};
