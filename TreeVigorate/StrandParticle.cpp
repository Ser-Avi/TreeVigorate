#include "StrandParticle.h"

StrandParticle::StrandParticle(int sharedIndex, glm::vec2 position, NodeHandle nodeHandle) : index(sharedIndex), localPosition(position), nodeHandle(nodeHandle)
{
}

StrandParticle::StrandParticle(glm::vec2 position, NodeHandle nodeHandle) : localPosition(position), index(indexCount), nodeHandle(nodeHandle) {
	indexCount++;
}

StrandParticle::StrandParticle()
{
}

int StrandParticle::getIndex() const {
	return index;
}

glm::vec2 StrandParticle::getLocalPosition() const {
	return localPosition;
}

const NodeHandle StrandParticle::getNodeHandle() const
{
	return nodeHandle;
}

void StrandParticle::setLocalPosition(glm::vec2 position) {
	localPosition = position;
}
