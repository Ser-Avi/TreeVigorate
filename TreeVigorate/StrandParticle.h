#pragma once

#include "glm/glm.hpp"
#include "Rhizocode/TreeModel.hpp"

using namespace EcoSysLab;

class StrandParticle {
public:
	StrandParticle(int sharedIndex, Node<InternodeGrowthData>& node, glm::vec2 position);
	StrandParticle(Node<InternodeGrowthData>& node, glm::vec2 position);

	inline static int indexCount = 0;
	int getIndex() const;
private:
	const int index;
	glm::vec2 localPosition;
	const Node<InternodeGrowthData>& node;
};