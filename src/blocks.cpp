#include "blocks.hpp"

#include <vector>
#include <memory>

#include "textures.hpp"

Block::~Block() { }

uint8_t Block::getFaceTexture(uint8_t face) {
	return _mainTexture;
}

BlockId Block::id() { return _id; }
bool Block::isOpaqueCube() { return _isOpaqueCube; }
uint8_t Block::mainTexture() { return _mainTexture; }

Block& Block::fromId(BlockId id) {
	return BlockRegistry::fromId(id);
}


Block::Block() : _id(BlockRegistry::nextId()), _isOpaqueCube(true), _mainTexture(TEX(PLACEHOLDER)) { }

Block& Block::isOpaqueCube(bool isOpaqueCube) { _isOpaqueCube = isOpaqueCube; return *this; }
Block& Block::mainTexture(uint8_t texture) { _mainTexture = texture; return *this; }


namespace BlockRegistry {
	namespace {
		std::vector<std::unique_ptr<Block>> protoBlocks;
		
		Block& registerBlock(Block* block) {
			protoBlocks.emplace_back(std::unique_ptr<Block>(block));
			return *block;
		}
	}
	
	BlockId nextId() {
		return protoBlocks.size() + 1;
	}
	
	void registerBlocks() {
		registerBlock(new Block()).mainTexture(TEX(STONE)); // stone
		registerBlock(new Block()).mainTexture(TEX(DIRT)); // dirt
		registerBlock(new GrassBlock()); // grass
		registerBlock(new TrunkBlock()); // tree trunk
		registerBlock(new Block()).mainTexture(TEX(LEAVES)); // leaves
	}

	Block& fromId(BlockId id) {
		return *protoBlocks[id - 1];
	}
	
	unsigned int registeredIds() {
		return protoBlocks.size();
	}
}

uint8_t GrassBlock::getFaceTexture(uint8_t face) {
	if(face == 4) {
		return TEX(DIRT);
	} else if(face == 5) {
		return TEX(GRASS_TOP);
	} else {
		return TEX(GRASS_SIDE);
	}
}

uint8_t TrunkBlock::getFaceTexture(uint8_t face) {
	return face >= 4 ? TEX(TRUNK_INSIDE) : TEX(TRUNK_SIDE);
}
