#pragma once

#include "blocks.hpp"
#include "face_renderer.hpp"

class Hotbar {
public:
	Hotbar(FaceRenderer& renderer);
	void init();
	
	void previous();
	void next();
	BlockId held();
	
	void render();
	
private:
	BlockId _held;
	FaceRenderer& faceRenderer;
	FaceBuffer buffer;
	
	void prerender();
};
