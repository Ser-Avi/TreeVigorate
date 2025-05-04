#pragma once
#include "glm/glm.hpp"
#include <StrandParticle.h>
#include <util.h>
#include <memory>
#include <unordered_map>
#include <DelauneyData.h>

using namespace EcoSysLab;


class StrandManager {
	std::unordered_map<NodeHandle, std::vector<StrandParticle>> nodeToParticlesMap;
	/// <summary>
	/// For descendent of a new node, generate corresponding strand particles.
	/// Recursive.
	/// </summary>
	/// <param name="sourceNode"></param> source node, not the new node but some ancestor of the new node.
	/// <param name="skeleton"></param> graph
	/// <param name="particleIndices"></param> corresponding indices to new particles
	
	void populateStrandsFromChildNode(Node<InternodeGrowthData>& childNode, Node<InternodeGrowthData>& receiverNode, ShootSkeleton& skeleton, std::vector<int> particleIndices, float r);
	/// <summary>
	/// Given source node, populate through to root new particles with a shared index
	/// </summary>
	void populateStrandsFromNode(Node<InternodeGrowthData>& sourceNode, ShootSkeleton& skeleton, int particleCount, float r);

public:
	//std::vector<StrandParticle&> getParticlesAtNode(const Node<InternodeGrowthData>& node) const;

	inline static float boundaryAttractantCoefficient = 0.1f;
	/// <summary>
	/// For all nodes in skeleton where parent == null and there is no corresponding particles, generate n particles through to root
	/// </summary>
	/// <param name="skeleton"></param>
	void generateParticlesForTree(ShootSkeleton& skeleton, int n, float r);

	void resolvePbd(ShootSkeleton& skeleton, NodeHandle handle, DelauneyData delauneyData, float r);

	std::unordered_map<int,glm::vec3> getGlobalNodeParticlePositions(NodeHandle handle, ShootSkeleton& skeleton);

	DelauneyData getPlaneTriangleIdx(NodeHandle handle, float maxEdge, int indexOffset);

	std::vector<int> getBridgeTriangleIdx(ShootSkeleton& skeleton, std::unordered_map<NodeHandle, DelauneyData> nodeToDelauneyData);

	std::unordered_map<NodeHandle, std::vector<StrandParticle>> getNodeToParticlesMap() const;
};