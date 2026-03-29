#include "SVDAG.h"

#include <cstring>
#include <iostream>

#include "tracy/Tracy.hpp"
#include "xxhash.h"
#include "../../../../cmake-build-debug/_deps/tracy-src/public/tracy/TracyC.h"

#include "client/VoxelGameClient.h"
#include "client/registry/BlockMaterials.h"
#include "common/registry/Blocks.h"

namespace voxel_game::client::renderer::universe {
	SVDAG::SVDAG(UniverseRenderer *renderer, voxel_game::universe::block::BlockObject *object, const glm::ivec3 pos) : mChunkID(renderer->getUnusedChunkID()), mObject(object), mPos(pos) {
		VoxelGameClient::getClient()->getThreadPool()->scheduleTaskNow(10, [this, renderer] {
			ZoneScopedN("Build SVDAG");
			buildSVDAG(renderer, mPos);
		});
	}

	void SVDAG::buildSVDAG(UniverseRenderer *renderer, const glm::ivec3 min) {
		TracyCZoneN(svdagBuildZone, "Build main SVDAG", 1);
		mOffset = buildSVDAG_(renderer, min, 256);
		TracyCZoneEnd(svdagBuildZone);

		ZoneScopedN("Upload additional data");
		Allocation materialIndexAllocation = renderer->getMaterialIndexBuffer()->allocate(mMaterialIndices.size() * sizeof(uint32_t));
		auto materialIndexBuffer = reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(renderer->getMaterialIndexBuffer()->getBuffer().allocationInfo.pMappedData) + materialIndexAllocation.offset);
		std::memcpy(materialIndexBuffer, mMaterialIndices.data(), mMaterialIndices.size() * sizeof(uint32_t));

		auto metadata = reinterpret_cast<ChunkMetadata*>(static_cast<uint8_t*>(renderer->getChunkMetadataBuffer().allocationInfo.pMappedData) + mChunkID * sizeof(ChunkMetadata));
		metadata->rootOffset = mOffset;
		metadata->attributeOffset = materialIndexAllocation.offset / sizeof(uint32_t);
		metadata->attributeCount = mMaterialIndices.size();
		uint32_t objectID = mChunkID;
		metadata->objectID = objectID;

		auto modelBuffer = reinterpret_cast<glm::mat4*>(static_cast<uint8_t*>(renderer->getModelBuffer().allocationInfo.pMappedData) + objectID * 2 * sizeof(glm::mat4));
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(mPos));
		modelBuffer[0] = model;
		modelBuffer[1] = glm::inverse(model);

		struct ChunkData {
			glm::vec4 minAABB;
			glm::vec4 maxAABB;
			glm::vec4 worldPos;
		};

		auto chunkBufferData = static_cast<ChunkData*>(renderer->getChunkBuffer().allocationInfo.pMappedData);
		chunkBufferData[mChunkID].minAABB = glm::vec4(mPos, 1.0f);
		chunkBufferData[mChunkID].maxAABB = glm::vec4(mPos + glm::ivec3(256), 1.0f);
		chunkBufferData[mChunkID].worldPos = glm::vec4(mPos, 1.0f);
	}

	uint32_t SVDAG::buildSVDAG_(UniverseRenderer *renderer, const glm::ivec3 min, const int32_t size) {
		if (size == 4) {
			return createLeafNode(renderer, min);
		}

		int32_t half = size / 2;
		uint8_t currentMask = 0;
		uint32_t children[8] = {};
		bool same = true;
		for (uint32_t i = 0; i < 8; i++) {
			glm::ivec3 childMin = min + SVDAG_LOOKUP_OFFSET[i] * half;
			uint32_t uniformMaterial = getUniformMaterial(childMin, half);
			if (uniformMaterial != UINT32_MAX) {
				children[i] = uniformMaterial == 0? 0: 0x80000000 | uniformMaterial;
			}
			else {
				children[i] = buildSVDAG_(renderer, childMin, half);
				same = false;
			}

			if (children[i] != 0) {
				currentMask |= 1 << i;
			}
			if (i > 0 && children[i] != children[i-1]) {
				same = false;
			}
		}

		if (same && (children[0] & 0x80000000 || children[0] == 0)) {
			return children[0];
		}

		struct {
			uint8_t mask;
			uint32_t children[8];
		} nodeData = {};
		nodeData.mask = currentMask;
		std::memcpy(nodeData.children, children, 8 * sizeof(uint32_t));
		uint64_t hash = XXH3_64bits(&nodeData, sizeof(nodeData));
		auto it = mNodes.find(hash);
		if (it != mNodes.end()) {
			uint32_t address = it->second.offset;
			mReferenceCounts[address]++;
			return address;
		}

		Allocation allocation = renderer->getSVDAGBuffer()->allocate(36);
		auto *buffer = reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(renderer->getSVDAGBuffer()->getBuffer().allocationInfo.pMappedData) + allocation.offset);
		*buffer = currentMask;
		// Use +1 to place children immediately after the 4-byte mask
		std::memcpy(buffer + 1, children, 8 * sizeof(uint32_t));

		mNodes[hash] = allocation;
		mReferenceCounts[allocation.offset]++;
		return allocation.offset;
	}

	uint32_t SVDAG::createLeafNode(const UniverseRenderer *renderer, const glm::ivec3 min) {
		voxel_game::universe::block::Chunk *chunk = mObject->getChunk(min >> 5);
		if (chunk == nullptr) {
			return 0;
		}
		uint64_t mask = 0;
		int32_t maskPos = 0;
		for (int32_t x = 0; x < 4; x++) {
			for (int32_t y = 0; y < 4; y++) {
				for (int32_t z = 0; z < 4; z++, maskPos++) {
					uint32_t block = chunk->getBlock(glm::ivec3{x, y, z} + min);
					if (block != 0) {
						mask |= 1ULL << maskPos;

						BlockMaterial *material = registry::block_materials::GRASS;
						uint32_t materialIndex = UINT32_MAX;
						for (uint32_t i = 0; i < mMaterials.size(); i++) {
							if (mMaterials[i] == material->getID()) {
								materialIndex = i;
								break;
							}
						}
						if (materialIndex == UINT32_MAX) {
							uint32_t index = mMaterials.size();
							mMaterials.push_back(material->getID());
							mMaterialIndices.push_back({mMaterialIndex++, 1, index});
						}
						else if (mMaterialIndices.back().material != materialIndex) {
							mMaterialIndices.push_back({mMaterialIndex++, 1, materialIndex});
						}
						else {
							mMaterialIndices.back().count++;
							mMaterialIndex++;
						}
					}
				}
			}
		}
		if (mask == 0) {
			return 0;
		}

		uint64_t hash = XXH3_64bits(&mask, sizeof(uint64_t));
		auto it = mNodes.find(hash);
		if (it != mNodes.end()) {
			return it->second.offset;
		}

		Allocation allocation = renderer->getSVDAGBuffer()->allocate(8);
		*reinterpret_cast<uint64_t*>(static_cast<uint8_t*>(renderer->getSVDAGBuffer()->getBuffer().allocationInfo.pMappedData) + allocation.offset) = mask;
		mNodes[hash] = allocation;
		return allocation.offset;
	}

	uint32_t SVDAG::getUniformMaterial(const glm::ivec3 min, const int32_t size) const {
		voxel_game::universe::block::Chunk *chunk = mObject->getChunk(min >> 5);
		if (chunk == nullptr) {
			return true;
		}
		glm::ivec3 chunkMin = min & 31;
		uint32_t firstBlock = chunk->getBlock(chunkMin);
		for (int32_t x = 0; x < size; x++) {
			for (int32_t y = 0; y < size; y++) {
				for (int32_t z = 0; z < size; z++) {
					if (chunk->getBlock(glm::ivec3{x, y, z} + chunkMin) != firstBlock) {
						return UINT32_MAX;
					}
				}
			}
		}
		return firstBlock;
	}
}
