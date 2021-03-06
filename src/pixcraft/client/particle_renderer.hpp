#pragma once

#include <vector>
#include <random>

#include "glfw.hpp"
#include "shaders.hpp"
#include "textures.hpp"

#include "../util/pairing_heap.hpp"

namespace PixCraft {
	struct Particle {
		float x, y, z;
		float size;
		TexId blockTex;
		float tx, ty;
		float vx, vy, vz;
		uint32_t deathTime;
		
		bool operator<(const Particle other) const;
	};
	
	class ParticleRenderer {
	public:
		void init();
		
		void spawnBlockBits(glm::vec3 blockPos, TexId blockTex);
		
		void update(float dt);
		
		void render(glm::mat4 proj, glm::mat4 view, float fovy, int height);
		
	private:
		const unsigned int MAX_PARTICLES = 512;
		
		ShaderProgram program;
		VertexBuffer<glm::vec3, float, TexId, glm::vec2> buffer;
		
		std::mt19937 random;
		
		PairingHeap<Particle> particles;
		uint32_t elapsedFrames;
	};
}
