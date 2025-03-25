#include "StrandManager.h"
#include <math.h>
#include <iostream>
#define M_PI       3.14159265358979323846   // pi

void StrandManager::populateStrandsFromNode(Node<InternodeGrowthData>& sourceNode, ShootSkeleton& skeleton, int particleCount) {
	if (nodeToParticlesMap.count(sourceNode.GetHandle()) > 0) {
		std::cout << "\nParticles already generated for node, ignoring.";
		return;
	}
	
	float radius = sourceNode.m_info.m_thickness;
	std::vector<int> generatedIndices;
	nodeToParticlesMap[sourceNode.GetHandle()] = std::vector<uPtr<StrandParticle>>();

;	for (int i = 0; i < particleCount; ++i) {
		float rad = 2* M_PI * (i / static_cast<float>(particleCount));
		glm::vec2 localPosition(glm::cos(rad) * radius, glm::sin(rad) * radius);

		uPtr<StrandParticle> particle = mkU<StrandParticle>(sourceNode, localPosition);
		generatedIndices.push_back(particle->getIndex());
		nodeToParticlesMap[sourceNode.GetHandle()].push_back(std::move(particle));
	}

	if (sourceNode.GetParentHandle() != -1) {
		Node<InternodeGrowthData>& parentNode = skeleton.RefNode(sourceNode.GetParentHandle());
		populateStrandsFromNode(parentNode, skeleton, generatedIndices);
	}
}

void StrandManager::populateStrandsFromNode(Node<InternodeGrowthData>& sourceNode, ShootSkeleton& skeleton, std::vector<int> particleIndices) {
	float radius = sourceNode.m_info.m_thickness;
	
	for (int i = 0; i < particleIndices.size(); ++i) {
		//todo: this is not how we position the descendents here, need to offset according to calling node position
		float rad = 2 * M_PI * (i / static_cast<float>(particleIndices.size()));
		glm::vec2 localPosition(glm::cos(rad) * radius, glm::sin(rad) * radius);

		uPtr<StrandParticle> particle = mkU<StrandParticle>(particleIndices[i], sourceNode, localPosition);
		nodeToParticlesMap[sourceNode.GetHandle()].push_back(std::move(particle));
	}

	if (sourceNode.GetParentHandle() != -1) {
		Node<InternodeGrowthData>& parentNode = skeleton.RefNode(sourceNode.GetParentHandle());
		populateStrandsFromNode(parentNode, skeleton, particleIndices);
	}
}

void StrandManager::generateParticlesForTree(ShootSkeleton& skeleton, int n) {
	std::vector<Node<InternodeGrowthData>> nodes = skeleton.RefRawNodes();
	for (int i = 0; i < nodes.size(); ++i) {
		auto& node = nodes[i];
		if (node.RefChildHandles().size() == 0) {
			this->populateStrandsFromNode(node, skeleton, n);
		}
	}
}

