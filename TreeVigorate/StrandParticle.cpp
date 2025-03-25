#include "StrandParticle.h"

StrandParticle::StrandParticle(int sharedIndex, Node<InternodeGrowthData>& node, glm::vec2 position) : index(sharedIndex), node(node), localPosition(position) 
{
}

StrandParticle::StrandParticle(Node<InternodeGrowthData>& node, glm::vec2 position) : node(node), localPosition(position), index(indexCount) {
	indexCount++;
}

int StrandParticle::getIndex() const {
	return index;
}