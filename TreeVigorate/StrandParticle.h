#pragma once

#include "glm/glm.hpp"
#include "Rhizocode/TreeModel.hpp"

using namespace EcoSysLab;

class StrandParticle {
public:
	StrandParticle(int sharedIndex, glm::vec2 position, NodeHandle nodeHandle);
	StrandParticle(glm::vec2 position, NodeHandle nodeHandle);
	StrandParticle();
	~StrandParticle() {}

	inline static int indexCount = 0;
	int getIndex() const;
	glm::vec2 getLocalPosition() const;
	const NodeHandle getNodeHandle() const;


	void setLocalPosition(glm::vec2 position);
private:
	int index;
	glm::vec2 localPosition;
	NodeHandle nodeHandle;
};