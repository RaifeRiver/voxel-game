#pragma once

#include <vector>

#include "BlockMaterial.h"
#include "client/renderer/AllocatableBuffer.h"
#include "common/universe/block/BlockObject.h"

namespace voxel_game::client::renderer::universe {
	class UniverseRenderer;

	static constexpr glm::ivec3 SVDAG_LOOKUP_OFFSET[8] = {
		{0, 0, 0}, // 0: Bottom-Front-Left
		{1, 0, 0}, // 1: Bottom-Front-Right
		{0, 1, 0}, // 2: Bottom-Back-Left
		{1, 1, 0}, // 3: Bottom-Back-Right
		{0, 0, 1}, // 4: Top-Front-Left
		{1, 0, 1}, // 5: Top-Front-Right
		{0, 1, 1}, // 6: Top-Back-Left
		{1, 1, 1}  // 7: Top-Back-Right
	};

	struct SVDAGMaterialIndex {
		uint32_t start;
		uint32_t count;
		uint32_t material;
	};

	class SVDAG {
	public:
		SVDAG(UniverseRenderer *renderer, voxel_game::universe::block::BlockObject *object, glm::ivec3 pos);

	private:
		std::unordered_map<uint64_t, Allocation> mNodes;
		std::unordered_map<uint32_t, uint32_t> mReferenceCounts;
		std::vector<SVDAGMaterialIndex> mMaterialIndices;
		std::vector<registry::RegistryID> mMaterials;
		uint32_t mMaterialIndex = 0;
		uint32_t mOffset = 0;
		uint16_t mChunkID = 0;

		voxel_game::universe::block::BlockObject *mObject;
		glm::ivec3 mPos;

		void buildSVDAG(UniverseRenderer *renderer, glm::ivec3 min);

		uint32_t buildSVDAG_(UniverseRenderer *renderer, glm::ivec3 min, int32_t size);

		uint32_t createLeafNode(const UniverseRenderer *renderer, glm::ivec3 min);

		uint32_t getUniformMaterial(glm::ivec3 min, int32_t size) const;
	};
}
