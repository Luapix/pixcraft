#pragma once

#include <unordered_map>
#include <unordered_set>

#include <stb_image.h>

#include "pixcraft/server/world.hpp"
#include "face_renderer.hpp"
#include "view_frustum.hpp"

namespace PixCraft {
	class RenderedChunk {
	public:
		void init(World& world, FaceRenderer& faceRenderer, int32_t chunkX, int32_t chunkZ);
		bool isInitialized();
		
		void prerender();
		void updateBuffers();
		
		void updateBlock(int8_t relX, int8_t y, int8_t relZ);
		void updatePlaneX(int8_t relX);
		void updatePlaneZ(int8_t relZ);
		
		void render(FaceRenderer& faceRenderer);
		void renderTranslucent(FaceRenderer& faceRenderer);
		
	private:
		World* world;
		FaceBuffer buffer;
		FaceBuffer translucentBuffer;
		int32_t chunkX, chunkZ;
		
		void prerenderBlock(Chunk& chunk, uint8_t relX, uint8_t y, uint8_t relZ);
	};
	
	class ChunkRenderer {
	public:
		ChunkRenderer(World& world, FaceRenderer& renderer);
		
		bool isChunkRendered(int32_t chunkX, int32_t chunkZ);
		size_t renderedChunkCount();
		
		void reset();
		
		void updateBlocks();
		
		void render(int32_t camChunkX, int32_t chamChunkZ, int renderDist, ViewFrustum& vf);
		void renderTranslucent(int32_t camChunkX, int32_t chamChunkZ, int renderDist, ViewFrustum& vf);
		
	private:
		World& world;
		FaceRenderer& faceRenderer;
		
		std::unordered_map<uint64_t, RenderedChunk> renderedChunks;
		
		void prerenderChunk(std::unordered_set<uint64_t>& updated, int32_t chunkX, int32_t chunkZ);
		void updateBlock(std::unordered_set<uint64_t>& updated, int32_t x, int32_t y, int32_t z);
	};
}
