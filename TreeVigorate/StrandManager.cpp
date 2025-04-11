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
		populateStrandsFromChildNode(parentNode, sourceNode, skeleton, generatedIndices);
	}
}

void StrandManager::populateStrandsFromChildNode(Node<InternodeGrowthData>& childNode, Node<InternodeGrowthData>& receiverNode, ShootSkeleton& skeleton, std::vector<int> particleIndices) {
	float radius = receiverNode.m_info.m_thickness;
	
	glm::vec2 projectedVector = glm::vec2(childNode.m_info.m_globalPosition.x - receiverNode.m_info.m_globalPosition.x, childNode.m_info.m_globalPosition.z - receiverNode.m_info.m_globalPosition.z);
	float offsetRadii = childNode.m_info.m_thickness + receiverNode.m_info.m_thickness;
	glm::vec2 offsetVector = glm::normalize(projectedVector) * offsetRadii;

	std::vector<uPtr<StrandParticle>> childParticles = nodeToParticlesMap[childNode.GetHandle()];
	std::vector<uPtr<StrandParticle>> receiverParticles = nodeToParticlesMap[childNode.GetHandle()];

	for (auto& particle : childParticles) {
		auto newParticleIterator = std::find(particleIndices.front(), particleIndices.back(), particle.get()->getIndex());
		if (newParticleIterator != particleIndices.back()) {
			glm::vec2 calculatedPosition = particle.get()->getLocalPosition() + offsetVector;
			uPtr<StrandParticle> newParentParticle = mkU<StrandParticle>(particle.get()->getIndex(), receiverNode, calculatedPosition);
			nodeToParticlesMap[receiverNode.GetHandle()].push_back(std::move(particle));
		}
	}

	if (receiverNode.GetParentHandle() != -1) {
		Node<InternodeGrowthData>& parentNode = skeleton.RefNode(receiverNode.GetParentHandle());
		populateStrandsFromChildNode(receiverNode, parentNode, skeleton, particleIndices);
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

void StrandManager::resolvePbd(ShootSkeleton& skeleton) {
	for (auto& pair : nodeToParticlesMap) {
		
	}


}

