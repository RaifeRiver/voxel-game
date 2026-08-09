#pragma once

#include <cstdint>

// ReSharper disable once CppTemplateParameterNeverUsed
template <typename T> struct Component {
	[[nodiscard]] static uint32_t getID() {
		static uint32_t id = sNextID++;
		return id;
	}

private:
	static inline uint32_t sNextID = 0;
};
