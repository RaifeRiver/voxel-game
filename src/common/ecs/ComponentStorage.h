#pragma once

#include <concepts>
#include <stdexcept>
#include <vector>

#include "Component.h"
#include "Entity.h"

class IComponentStorage {};

template <typename T> requires std::derived_from<T, Component<T>> class ComponentStorage : public IComponentStorage {
public:
	T& attach(const Entity entity, T component = T{}) {
		if (has(entity)) {
			return mComponents[mComponentIndices[entity]];
		}
		if (mUnusedComponents.size() > 0) {
			uint32_t index = mUnusedComponents.back();
			mUnusedComponents.pop_back();
			mComponents[index] = component;
			return &mComponents[index];
		}
		uint32_t index = mComponents.size();
		mComponents.push_back(component);
		return &mComponents[index];
	}

	T& get(const Entity entity) {
		uint32_t index = mComponentIndices.at(entity);
		if (index == UINT32_MAX) {
			throw std::runtime_error("Entity does not have the requested component");
		}
		return &mComponents[index];
	}

	bool has(const Entity entity) const {
		return mComponentIndices.size() < entity && mComponentIndices[entity] != UINT32_MAX;
	}

	void remove(const Entity entity) {
		const uint32_t index = mComponentIndices.at(entity);
		if (index != UINT32_MAX) {
			mUnusedComponents.push_back(index);
		}
	}

private:
	std::vector<uint32_t> mComponentIndices;
	std::vector<T> mComponents;
	std::vector<uint32_t> mUnusedComponents;
};