#include "StrandManager.h"
#include <math.h>
#include <iostream>
#include <delaunator.hpp>
#define M_PI       3.14159265358979323846   // pi

void StrandManager::populateStrandsFromNode(Node<InternodeGrowthData>& sourceNode, ShootSkeleton& skeleton, int particleCount, float r) {
	if (nodeToParticlesMap.count(sourceNode.GetHandle()) > 0) {
		std::cout << "\nParticles already generated for node, ignoring.";
		return;
	}
	
	float radius = sourceNode.m_info.m_thickness * (r) / 2.f;
	std::vector<int> generatedIndices;

;	for (int i = 0; i < particleCount; ++i) {
		float rad = 2* M_PI * (i / static_cast<float>(particleCount));
		glm::vec2 localPosition(glm::cos(rad) * radius, glm::sin(rad) * radius);

		StrandParticle particle(localPosition, sourceNode.GetHandle());

		generatedIndices.push_back(particle.getIndex());
		auto & vec =  nodeToParticlesMap[sourceNode.GetHandle()];
		vec.push_back(particle);
	}

	if (sourceNode.GetParentHandle() != -1) {
		Node<InternodeGrowthData>& parentNode = skeleton.RefNode(sourceNode.GetParentHandle());
		populateStrandsFromChildNode(sourceNode, parentNode, skeleton, generatedIndices, r);
	}

	for (auto& particle : nodeToParticlesMap[sourceNode.GetHandle()]) {
		particle.setLocalPosition(particle.getLocalPosition());
	}
}

void StrandManager::populateStrandsFromChildNode(Node<InternodeGrowthData>& childNode, Node<InternodeGrowthData>& receiverNode, ShootSkeleton& skeleton, std::vector<int> particleIndices, float r) {	
	
	
	glm::vec2 projectedVector = glm::vec2(childNode.m_info.m_globalPosition.x - receiverNode.m_info.m_globalPosition.x, childNode.m_info.m_globalPosition.z - receiverNode.m_info.m_globalPosition.z);
	float offsetRadii = nodeToParticlesMap[receiverNode.GetHandle()].size() > 0 ? glm::pow(childNode.m_info.m_thickness + receiverNode.m_info.m_thickness,2) * glm::sqrt(r) : 0;
	glm::vec2 offsetVector = glm::normalize(projectedVector) * offsetRadii;
	
	std::vector<StrandParticle> childParticles = nodeToParticlesMap[childNode.GetHandle()];
	std::vector<StrandParticle> receiverParticles = nodeToParticlesMap[receiverNode.GetHandle()];
	std::vector<int> receiverParticleIndices;
	for (auto& particle : receiverParticles) {
		receiverParticleIndices.push_back(particle.getIndex());
	}


	for (auto& particle : childParticles) {
		auto newParticleIterator = std::find(receiverParticleIndices.begin(), receiverParticleIndices.end(), particle.getIndex());
		if (newParticleIterator == receiverParticleIndices.end()) {
			glm::vec2 calculatedPosition = particle.getLocalPosition();
			if (receiverNode.RefChildHandles().size() > 1) {
				//get maximum radii child. If this is not that child, offset.
				float maxRadius = 0;
				int maxRadiusNodeHandle = 0;
				for (auto& childHandle : receiverNode.RefChildHandles()) {
					if (skeleton.RefNode(childHandle).m_info.m_thickness * (1 + r) > maxRadius) {
						maxRadius = skeleton.RefNode(childHandle).m_info.m_thickness * (1 + r);
						maxRadiusNodeHandle = childHandle;
					}
				}
				if (maxRadiusNodeHandle != childNode.GetHandle()) {
					calculatedPosition + (offsetVector);
				}
			}
			StrandParticle newParentParticle(particle.getIndex(), calculatedPosition, receiverNode.GetHandle());
			nodeToParticlesMap[receiverNode.GetHandle()].push_back(newParentParticle);
		}
	}

	if (receiverNode.GetParentHandle() != -1) {
		Node<InternodeGrowthData>& parentNode = skeleton.RefNode(receiverNode.GetParentHandle());
		populateStrandsFromChildNode(receiverNode, parentNode, skeleton, particleIndices, r);
	}
}

void StrandManager::generateParticlesForTree(ShootSkeleton& skeleton, int n, float r) {
	std::vector<Node<InternodeGrowthData>> nodes = skeleton.RefRawNodes();
	for (int i = 0; i < nodes.size(); ++i) {
		auto& node = nodes[i];
		if (node.RefChildHandles().size() == 0) {
			this->populateStrandsFromNode(node, skeleton, n, r);
		}
	}
}

void StrandManager::resolvePbd(ShootSkeleton& skeleton, NodeHandle handle, DelauneyData delauneyData, float r){
	
	float boundary = skeleton.RefNode(handle).m_info.m_thickness * 1.5f * (1 + r);
	float particleCount = nodeToParticlesMap.size();
	float theta = 360.0 / particleCount;
	float desiredDistance = sqrt(2 * boundary * boundary * (1 - cos(theta * 0.01745)));

	std::unordered_map<int, StrandParticle*> particleIndexToParticle;
	for (auto& particle : nodeToParticlesMap[handle]) {
		particleIndexToParticle[particle.getIndex()] = &particle;
	}
	/*
	std::vector<int> triangleIndices = delauneyData.idx;
	for (int i = 0; i < triangleIndices.size() && false; ++i) {
		int particleIndex = delauneyData.vertIdToParticleIndex[triangleIndices[i]];
		//find forward edge pair
		int pairParticleIndex = delauneyData.vertIdToParticleIndex[triangleIndices[(i / 3) * 3 + (i + 1) % 3]];

		StrandParticle* vert = particleIndexToParticle[particleIndex];
		StrandParticle* nextVert = particleIndexToParticle[pairParticleIndex];

		float constraintMatched = glm::distance(vert->getLocalPosition(), nextVert->getLocalPosition()) - desiredDistance;
		glm::vec2 constraint1 = (nextVert->getLocalPosition() - vert->getLocalPosition()) / glm::distance(vert->getLocalPosition(), nextVert->getLocalPosition());
		glm::vec2 constraint2(-1 * constraint1.x, -1 * constraint1.y);

		float scalar = -1 * constraintMatched / (glm::pow(glm::length(constraint1),2) + glm::pow(glm::length(constraint2), 2));

		vert->setLocalPosition(vert->getLocalPosition() + scalar * constraint1);
		nextVert->setLocalPosition(nextVert->getLocalPosition() + scalar * constraint2);
	}
	*/
	
	for (auto& particle : nodeToParticlesMap[handle]) {
		float distance = glm::length(particle.getLocalPosition());

		if (distance > boundary) {
			glm::vec2 closestBoundaryPoint = boundary * particle.getLocalPosition() / distance;
			glm::vec2 newPosition = particle.getLocalPosition() + boundaryAttractantCoefficient * (closestBoundaryPoint - particle.getLocalPosition());

			particle.setLocalPosition(newPosition);
		}
		else {
				//todo: attractants
		}

	}
	
}

std::unordered_map<int, glm::vec3> StrandManager::getGlobalNodeParticlePositions(NodeHandle handle, ShootSkeleton& skeleton)
{
	std::unordered_map<int, glm::vec3> particleIdToPosition;
	for (auto& particle : nodeToParticlesMap[handle]) {
		glm::vec2 localPosition = particle.getLocalPosition();
		glm::vec3 globalPosition = skeleton.RefNode(handle).m_info.m_globalPosition;

		glm::vec3 addLocalPosition = glm::vec3(localPosition.x, 0, localPosition.y);
		
		if (skeleton.RefNode(handle).GetParentHandle() >= 0) {
			glm::vec3 parentVector = skeleton.RefNode(handle).m_info.m_globalPosition - skeleton.RefNode(skeleton.RefNode(handle).GetParentHandle()).m_info.m_globalPosition;
			addLocalPosition = glm::rotation(glm::vec3(0, 1, 0), glm::normalize(parentVector)) * addLocalPosition;
		}
		globalPosition += addLocalPosition;

		particleIdToPosition[particle.getIndex()] = (globalPosition);
	}

	return particleIdToPosition;
}
/*
std::size_t nextHalfedge(std::size_t i) {
	return (i % 3 == 2) ? i - 2 : i + 1;
}
std::size_t prevHalfedge(std::size_t i) {
	return (i % 3 == 0) ? i + 2 : i - 1;
}

bool noSym(std::size_t i, const std::vector<size_t>& HEs) {
	return HEs[i] < 0 || HEs[i] > HEs.size() + 1;
}
*/




DelauneyData StrandManager::getPlaneTriangleIdx(NodeHandle handle, float maxEdge, int indexOffset)
{
	std::vector<glm::vec2> points;
	std::unordered_map<int, int> vertIdToParticleIndex;
	std::unordered_map<int, int> particleIndexToVertId;

	int offset = 0;
	for (auto& point : nodeToParticlesMap[handle]) {
		points.push_back(point.getLocalPosition());
		vertIdToParticleIndex[indexOffset + offset] = point.getIndex();
		particleIndexToVertId[point.getIndex()] = indexOffset + offset;
		offset++;
	}

	std::vector<int> IDXs;

	// I. first we convert our vec2s to points that our delaunay algo reads
	std::vector<double> dPts;
	for (int i = 0; i < points.size(); ++i) {
		glm::vec2 p = points[i];
		dPts.push_back(p.x);
		dPts.push_back(p.y);
	}

	// II. triangulate
	delaunator::Delaunator d(dPts);
	
	// III. Remove triangles that have too long edges
	// we will store the valid HEs
	std::vector<int> validHEs;


	for (int i = 0; i < d.triangles.size(); i += 3) {
		// all points of this triangle
		double x0 = d.coords[2 * d.triangles[i]];        //tx0
		double y0 = d.coords[2 * d.triangles[i] + 1];    //ty0
		double x1 = d.coords[2 * d.triangles[i + 1]];    //tx1
		double y1 = d.coords[2 * d.triangles[i + 1] + 1];//ty1
		double x2 = d.coords[2 * d.triangles[i + 2]];    //tx2
		double y2 = d.coords[2 * d.triangles[i + 2] + 1]; //ty2
		// the tree edges, where eij -> i to j
		glm::vec2 e01 = glm::vec2(x1, y1) - glm::vec2(x0, y0);
		glm::vec2 e12 = glm::vec2(x2, y2) - glm::vec2(x1, y1);
		glm::vec2 e20 = glm::vec2(x0, y0) - glm::vec2(x2, y2);

		bool isLong = glm::length(e01) > maxEdge || glm::length(e12) > maxEdge || glm::length(e20) > maxEdge;

		if (!isLong) {
			validHEs.push_back(i);
			validHEs.push_back(i + 1);
			validHEs.push_back(i + 2);
		}
	}

	for (const auto& tri : d.triangles) {
			IDXs.push_back((int)tri + indexOffset);
	}
		
		// if we want to change this back to return points on a convex hull,
		// this is how:

	std::size_t curr = d.hull_start;
	std::vector<int> hullIdx;

	int closestToZeroHullIndex = 0;
	float smallestDegree = M_PI * 2;

	do {
		hullIdx.push_back(curr + indexOffset);
		curr = d.hull_next[curr]; // Move to next vertex

		float polarDegree = glm::abs(glm::atan(nodeToParticlesMap[handle][curr].getLocalPosition().y, nodeToParticlesMap[handle][curr].getLocalPosition().x));
		if (polarDegree < smallestDegree) {
			smallestDegree = polarDegree;
			closestToZeroHullIndex = hullIdx.size()-1;
		}
	} while (curr != d.hull_start); // Loop until we return to start

	auto startIterator = hullIdx.begin() + closestToZeroHullIndex;
	auto endIterator = hullIdx.end();

	std::vector<int> polarSortedHull(startIterator, endIterator);
	for (int i = 0; i < closestToZeroHullIndex; ++i) {
		polarSortedHull.push_back(hullIdx[i]);
	}

	return DelauneyData(IDXs, hullIdx, vertIdToParticleIndex, particleIndexToVertId);
}

std::vector<int> buildBridge(std::vector<int> parentLoop, std::vector<int> childLoop) {
	int parentCurr = 0;
	int childCurr = 0;

	int firstParent = parentCurr;
	int firstChild = childCurr;

	bool freezeParent = false;
	bool freezeChild = false;

	std::vector<int> bridgeTriangleIndices;
	//iterate parent, make triangle with prev
	do {
		if (!freezeParent) {
			int nextParent = (parentCurr + 1) % parentLoop.size();
			bridgeTriangleIndices.push_back(childLoop[childCurr]);
			bridgeTriangleIndices.push_back(parentLoop[parentCurr]);
			bridgeTriangleIndices.push_back(parentLoop[nextParent]);

			parentCurr = nextParent;
			if (parentCurr == firstParent) freezeParent = true;
		}

		if (!freezeChild) {
			int nextChild = (childCurr + 1) % childLoop.size();
			bridgeTriangleIndices.push_back(parentLoop[parentCurr]);
			bridgeTriangleIndices.push_back(childLoop[nextChild]);
			bridgeTriangleIndices.push_back(childLoop[childCurr]);

			childCurr = nextChild;
			if (childCurr == firstChild) freezeChild = true;
		}


	} while (!freezeParent || !freezeChild);

	return bridgeTriangleIndices;
}

std::vector<int> StrandManager::getBridgeTriangleIdx(ShootSkeleton& skeleton, std::unordered_map<NodeHandle, DelauneyData> nodeToDelauneyData)
{
	std::vector<int> bridgeTriangleIndices;
	for (auto& node : skeleton.RefRawNodes()) {
		NodeHandle handle = node.GetHandle();
		switch (skeleton.RefNode(handle).RefChildHandles().size()) {
		case 0: {
			//end branch leave as is.
			break;
		}
		case 1: {
			//Node<InternodeGrowthData>& parentNode = treeModel.RefShootSkeleton().RefNode(handle.GetHandle());
			//Node<InternodeGrowthData>& childNode = treeModel.RefShootSkeleton().RefNode(handle.RefChildHandles()[0]);
			//okay, assuming we can get boundary verts
			std::vector<int> parentLoop = nodeToDelauneyData[handle].polarHullIdx;
			std::vector<int> childLoop = nodeToDelauneyData[node.RefChildHandles()[0]].polarHullIdx;

			//TODO: order by polar coordinates, starting at pointer to each closest to 0 degrees.
			std::vector<int> newBridgeTriangles = buildBridge(parentLoop, childLoop);
			//bridgeTriangleIndices.insert(bridgeTriangleIndices.end(), newBridgeTriangles.begin(), newBridgeTriangles.end());

			break;
		}
		default: {
			/*
			std::unordered_map<int, StrandParticle> parentParticleIdToParticle;
			for (auto& particle : nodeToParticlesMap[handle]) {
				parentParticleIdToParticle[particle.getIndex()] = particle;
			}
			DelauneyData parentData = nodeToDelauneyData[handle];

			for(int i = 0; i < 1; ++i){
			for (auto& child : skeleton.RefNode(handle).RefChildHandles()) {
				DelauneyData childData = nodeToDelauneyData[child];
				std::vector<StrandParticle> matchedParentParticles;
				//index of dPts will correspond to matchedParentParticles
				std::vector<double> dPts;

				for (auto& childParticle : childData.vertIdToParticleIndex) {
					int particleIndex = childParticle.second;
					matchedParentParticles.push_back(parentParticleIdToParticle[particleIndex]);

					dPts.push_back(parentParticleIdToParticle[particleIndex].getLocalPosition().x);
					dPts.push_back(parentParticleIdToParticle[particleIndex].getLocalPosition().y);
				}
				
				// II. triangulate
				delaunator::Delaunator d(dPts);

				//get hull on parent corresponding to child particle hull
				std::size_t curr = d.hull_start;
				std::vector<int> parentSubHull;

				//for determining firstPolarCoord:
				int closestToZeroHullIndex = 0;
				float smallestDegree = M_PI * 2;
				do {
					StrandParticle matchedParticle = matchedParentParticles[curr];
					int trueVertexId = parentData.particleIndexToVertId[matchedParticle.getIndex()];
					parentSubHull.push_back(trueVertexId);

					float polarDegree = glm::abs(glm::atan(matchedParticle.getLocalPosition().y, matchedParticle.getLocalPosition().x));
					if (polarDegree < smallestDegree) {
						smallestDegree = polarDegree;
						closestToZeroHullIndex = parentSubHull.size() - 1;
					}

					curr = d.hull_next[curr]; // Move to next vertex
				} while (curr != d.hull_start); // Loop until we return to start
				

				//shift parent sub hull for polar sorted hull
				auto startIterator = parentSubHull.begin() + closestToZeroHullIndex;
				auto endIterator = parentSubHull.end();

				std::vector<int> polarSortedHull(startIterator, endIterator);
				for (int i = 0; i < closestToZeroHullIndex; ++i) {
					polarSortedHull.push_back(parentSubHull[i]);
				}

				std::vector<int> newBridgeTriangles = buildBridge(parentSubHull, childData.polarHullIdx);
				bridgeTriangleIndices.insert(bridgeTriangleIndices.end(), newBridgeTriangles.begin(), newBridgeTriangles.end());
			}
			*/
		}
		}
	}
	return bridgeTriangleIndices;
}

std::unordered_map<NodeHandle, std::vector<StrandParticle>> StrandManager::getNodeToParticlesMap() const
{
	return nodeToParticlesMap;
}


