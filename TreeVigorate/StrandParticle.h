#pragma once

#include "glm/glm.hpp"
#include "Rhizocode/TreeModel.hpp"

using namespace EcoSysLab;

class StrandParticle {
public:
	StrandParticle(int sharedIndex, glm::vec2 position, NodeHandle nodeHandle);
	StrandParticle(glm::vec2 position, NodeHandle nodeHandle);
	~StrandParticle() {}

	inline static int indexCount = 0;
	int getIndex() const;
	glm::vec2 getLocalPosition() const;
	glm::vec2 getVelocity() const;
	const NodeHandle getNodeHandle() const;


	void setLocalPosition(glm::vec2 position);
	void setVelocity(glm::vec2 vel);

private:
	const int index;
	glm::vec2 localPosition;
	glm::vec2 velocity;
	const NodeHandle nodeHandle;
};