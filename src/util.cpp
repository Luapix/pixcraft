#include "util.hpp"

const int sideVectors[6][3] = {
	{0,0,1}, {1,0,0}, {0,0,-1}, {-1,0,0}, {0,-1,0}, {0,1,0}
};

glm::mat4 globalToLocalRot(glm::vec3 orient) {
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::rotate(mat, -orient.z, glm::vec3(0.0f, 0.0f, 1.0f));
	mat = glm::rotate(mat, -orient.x, glm::vec3(1.0f, 0.0f, 0.0f));
	mat = glm::rotate(mat, -orient.y, glm::vec3(0.0f, 1.0f, 0.0f));
	return mat;
}

glm::mat4 localToGlobalRot(glm::vec3 orient) {
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::rotate(mat, orient.y, glm::vec3(0.0f, 1.0f, 0.0f));
	mat = glm::rotate(mat, orient.x, glm::vec3(1.0f, 0.0f, 0.0f));
	mat = glm::rotate(mat, orient.z, glm::vec3(0.0f, 0.0f, 1.0f));
	return mat;
}

glm::mat4 globalToLocal(glm::vec3 pos, glm::vec3 orient) {
	glm::mat4 mat = globalToLocalRot(orient);
	mat = glm::translate(mat, -pos);
	return mat;
}

glm::mat4 localToGlobal(glm::vec3 pos, glm::vec3 orient) {
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::translate(mat, pos);
	mat = glm::rotate(mat, orient.y, glm::vec3(0.0f, 1.0f, 0.0f));
	mat = glm::rotate(mat, orient.x, glm::vec3(1.0f, 0.0f, 0.0f));
	mat = glm::rotate(mat, orient.z, glm::vec3(0.0f, 0.0f, 1.0f));
	return mat;
}


std::string vec3ToString(glm::vec3 pos) {
	const size_t bufSize = 64;
	char buffer[bufSize];
	snprintf(buffer, bufSize, "(%7.2f; %7.2f; %7.2f)", pos.x, pos.y, pos.z);
	return std::string(buffer);
}


SpiralIterator::SpiralIterator(int startX, int startZ)
	: startX(startX), startZ(startZ), x(0), z(0), dx(1), dz(0) { }

int SpiralIterator::getX() { return startX + x; }
int SpiralIterator::getZ() { return startZ + z; }

bool SpiralIterator::withinDistance(int dist) {
	return abs(x) <= dist && abs(z) <= dist;
}

void SpiralIterator::next() {
	x += dx;
	z += dz;
	if(x == z || (x < 0 && x == -z) || (x > 0 && x == 1-z)) {
		int dz0 = dz;
		dz = dx;
		dx = -dz0;
	}
}


int sign(float x) {
	if(x > 0) return 1;
	else if(x < 0) return -1;
	else return 0;
}

int getBlockCoordAt(float x) {
	return (int) round(x);
}

Ray::Ray(glm::vec3 startPos, glm::vec3 dir)
	: tDeltaX(1 / abs(dir.x)), tDeltaY(1 / abs(dir.y)), tDeltaZ(1 / abs(dir.z)),
	  stepX(sign(dir.x)), stepY(sign(dir.y)), stepZ(sign(dir.z)),
	  x(getBlockCoordAt(startPos.x)), y(getBlockCoordAt(startPos.y)), z(getBlockCoordAt(startPos.z)),
	  dist(0.0f), lastFace(0) {
	if(dir.x > 0) {
		tMaxX = (x + 0.5 - startPos.x) / dir.x;
	} else {
		tMaxX = (x - 0.5 - startPos.x) / dir.x;
	}
	if(dir.y > 0) {
		tMaxY = (y + 0.5 - startPos.y) / dir.y;
	} else {
		tMaxY = (y - 0.5 - startPos.y) / dir.y;
	}
	if(dir.z > 0) {
		tMaxZ = (z + 0.5 - startPos.z) / dir.z;
	} else {
		tMaxZ = (z - 0.5 - startPos.z) / dir.z;
	}
}

int Ray::getX() { return x; }
int Ray::getY() { return y; }
int Ray::getZ() { return z; }
float Ray::getDistance() { return dist; }
int Ray::getLastFace() { return lastFace; }

void Ray::nextFace() {
	if(tMaxX < tMaxY && tMaxX < tMaxZ) { // next face on X axis
		tMaxX += tDeltaX;
		x += stepX;
		lastFace = 2 + stepX; // west or east faces
	} else if(tMaxY < tMaxX && tMaxY < tMaxZ) { // next face on Y axis
		dist = tMaxY;
		tMaxY += tDeltaY;
		y += stepY;
		lastFace = 4 + (1-stepY)/2; // bottom or top faces
	} else { // next face on Z axis
		dist = tMaxZ;
		tMaxZ += tDeltaZ;
		z += stepZ;
		lastFace = 1 + stepZ;
	}
}


std::tuple<int,int,int> getBlockCoordsAt(glm::vec3 pos) {
	return std::tuple<int,int,int>(getBlockCoordAt(pos.x), getBlockCoordAt(pos.y), getBlockCoordAt(pos.z));
}

bool intervalIntersect(float x1, float x2, float y1, float y2) {
	return y1 < x2 && x1 < y2;
}

int clampInt(int x, int min, int max) {
	if(x < min) return min;
	if(x > max) return max;
	return x;
}

glm::vec3 snapToEdge(glm::vec3 pos, float radius, int snapX, int snapY, int snapZ) {
	const float yRadius = 0.001;
	if(snapX == 1) {
		pos.x = ceil(pos.x - radius + 0.5) + radius - 0.5;
	} else if(snapX == -1) {
		pos.x = floor(pos.x + radius + 0.5) - radius - 0.5;
	}
	if(snapY == 1) {
		pos.y = ceil(pos.y - yRadius + 0.5) + yRadius - 0.5;
	} else if(snapY == -1) {
		pos.y = floor(pos.y + yRadius + 0.5) - yRadius - 0.5;
	}
	if(snapZ == 1) {
		pos.z = ceil(pos.z - radius + 0.5) + radius - 0.5;
	} else if(snapZ == -1) {
		pos.z = floor(pos.z + radius + 0.5) - radius - 0.5;
	}
	return pos;
}

int collateCollisions(int a, int b, int c) {
	return (a == 1 || b == 1 || c == 1) - (a == -1 || b == -1 || c == -1);
}
