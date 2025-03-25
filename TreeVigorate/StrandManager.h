#pragma once
#include "glm/glm.hpp"
#include <StrandParticle.h>
#include <util.h>
#include <memory>

using namespace EcoSysLab;


class StrandManager {
	std::unordered_map<NodeHandle, std::vector<uPtr<StrandParticle>>> nodeToParticlesMap;
	/// <summary>
	/// For descendent of a new node, generate corresponding strand particles.
	/// Recursive.
	/// </summary>
	/// <param name="sourceNode"></param> source node, not the new node but some ancestor of the new node.
	/// <param name="skeleton"></param> graph
	/// <param name="particleIndices"></param> corresponding indices to new particles
	
	void populateStrandsFromNode(Node<InternodeGrowthData>& sourceNode, ShootSkeleton& skeleton, std::vector<int> particleIndices);
	/// <summary>
	/// Given source node, populate through to root new particles with a shared index
	/// </summary>
	void populateStrandsFromNode(Node<InternodeGrowthData>& sourceNode, ShootSkeleton& skeleton, int particleCount);
public:
	//std::vector<StrandParticle&> getParticlesAtNode(const Node<InternodeGrowthData>& node) const;


	/// <summary>
	/// For all nodes in skeleton where parent == null and there is no corresponding particles, generate n particles through to root
	/// </summary>
	/// <param name="skeleton"></param>
	void generateParticlesForTree(ShootSkeleton& skeleton, int n);
};