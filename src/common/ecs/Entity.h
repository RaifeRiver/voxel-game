#pragma once

#include <cstdint>

class Entity {
public:
	// ReSharper disable once CppNonExplicitConversionOperator
	operator uint32_t() const {
		return mID;
	}

private:
	uint32_t mID;

	explicit Entity(const uint32_t id) : mID(id) {}

	friend class Registry;
};