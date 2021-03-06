#include "player.hpp"

#include <cmath>
#include <iostream>

#include "pixcraft/util/util.hpp"

#include "blocks.hpp"
#include "world.hpp"

using namespace PixCraft;

const char* PixCraft::movementModeNames[3] = {
	"Normal", "Flying", "Noclip"
};

const float WAIST_HEIGHT = 1.05;
const float EYE_HEIGHT = 1.6;
const float HEIGHT = 1.7;
const float RADIUS = 0.2;

const float WALK_SPEED = 5.0f;
const float FLY_SPEED = 10.0f;
const float NOCLIP_SPEED = 30.0f;
const float SWIM_SPEED = 5.0f;
const float SWIM_ACCEL = 40.0f;

const float JUMP_HEIGHT = 1.3f;
const float JUMP_SPEED = sqrt(2*JUMP_HEIGHT*GRAVITY);


Player::Player(World& world, glm::vec3 pos)
	: Mob(world, HEIGHT, RADIUS, false, true, pos, glm::vec3(0.0)), _movementMode(MovementMode::normal) { }


glm::vec3 Player::eyePos() { return _pos + glm::vec3(0, EYE_HEIGHT, 0); }

bool Player::isEyeUnderwater() {
	int32_t x, y, z;
	std::tie(x, y, z) = getBlockCoordsAt(eyePos());
	Block* block = world.getBlock(x, y, z);
	return block == &Block::fromId(BlockRegistry::WATER_ID);
}

std::tuple<bool, int,int,int> Player::castRay(float maxDist, bool offset, bool hitFluids) {
	return world.raycast(eyePos(), dirVector(), maxDist, offset, hitFluids);
}


MovementMode Player::movementMode() { return _movementMode; }
void Player::movementMode(MovementMode mode) {
	_movementMode = mode;
	canFly = _movementMode == MovementMode::flying || _movementMode == MovementMode::noClip;
	collidesWithBlocks = _movementMode == MovementMode::normal || _movementMode == MovementMode::flying;
}


float Player::getMaxHorSpeed() {
	if(_movementMode == MovementMode::normal) {
		return WALK_SPEED;
	} else if(_movementMode == MovementMode::flying) {
		return FLY_SPEED;
	} else {
		return NOCLIP_SPEED;
	}
}


void Player::handleKeys(std::tuple<int,int,bool,bool> mvtKeys, float dt) {
	glm::mat4 yRot = glm::rotate(glm::mat4(1.0f), _orient.y, glm::vec3(0.0f, 1.0f, 0.0f));
	
	int dx, dz; bool up, down;
	std::tie(dx, dz, up, down) = mvtKeys;
	float mvtSpeed = getMaxHorSpeed();
	
	if(canFly) {
		int dy = up - down;
		if(dx != 0 || dy != 0 || dz != 0) {
			_speed = glm::normalize(glm::vec3(dx, dy, dz));
			_speed = mvtSpeed * glm::vec3(yRot * glm::vec4(_speed, 1.0f));
		} else {
			_speed = glm::vec3(0);
		}
	} else {
		glm::vec3 horSpeed(0);
		if(dx != 0 || dz != 0) {
			horSpeed = glm::normalize(glm::vec3(dx, 0, dz));
			horSpeed = mvtSpeed * glm::vec3(yRot * glm::vec4(horSpeed, 1.0f));
		} else {
			horSpeed = glm::vec3(0);
		}
		_speed.x = horSpeed.x; _speed.z = horSpeed.z;
		float waterHeight = getWaterHeight();
		if(up) {
			if(onGround && waterHeight < WAIST_HEIGHT) {
				_speed.y = JUMP_SPEED;
			}
			if(waterHeight >= WAIST_HEIGHT && _speed.y <= SWIM_SPEED) { // above waist
				_speed.y += dt*SWIM_ACCEL;
				if(_speed.y >= SWIM_SPEED) _speed.y = SWIM_SPEED;
			}
		}
	}
}

flatbuffers::Offset<void> Player::serialize(flatbuffers::FlatBufferBuilder& builder) {
	auto mobBase = serializeMobBase(builder);
	Serializer::MovementMode movementMode;
	switch(_movementMode) {
	case MovementMode::normal: movementMode = Serializer::MovementMode_Normal; break;
	case MovementMode::flying: movementMode = Serializer::MovementMode_Flying; break;
	case MovementMode::noClip: movementMode = Serializer::MovementMode_NoClip; break;
	}
	return Serializer::CreatePlayer(builder, mobBase, movementMode).Union();
}

uint8_t Player::serializedType() {
	return Serializer::Mob_Player;
}

std::unique_ptr<Player> Player::unserialize(World& world, const Serializer::Player* playerData) {
	std::unique_ptr<Player> player(new Player(world, glm::vec3(0.0,0.0,0.0)));
	player->unserializeMobBase(playerData->mob());
	switch(playerData->movement_mode()) {
	case Serializer::MovementMode_Normal: player->movementMode(MovementMode::normal); break;
	case Serializer::MovementMode_Flying: player->movementMode(MovementMode::flying); break;
	case Serializer::MovementMode_NoClip: player->movementMode(MovementMode::noClip); break;
	}
	return player;
}
