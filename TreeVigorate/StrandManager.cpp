#include "StrandManager.h"
#include <math.h>
#include <iostream>
#include <delaunator.hpp>
#define M_PI       3.14159265358979323846   // pi

void StrandManager::populateStrandsFromNode(Node<InternodeGrowthData>& sourceNode, ShootSkeleton& skeleton, int particleCount) {
	if (nodeToParticlesMap.count(sourceNode.GetHandle()) > 0) {
		std::cout << "\nParticles already generated for node, ignoring.";
		return;
	}
	
	float radius = sourceNode.m_info.m_thickness;
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
		populateStrandsFromChildNode(sourceNode, parentNode, skeleton, generatedIndices);
	}
}

void StrandManager::populateStrandsFromChildNode(Node<InternodeGrowthData>& childNode, Node<InternodeGrowthData>& receiverNode, ShootSkeleton& skeleton, std::vector<int> particleIndices) {
	float radius = receiverNode.m_info.m_thickness;
	
	glm::vec2 projectedVector = glm::vec2(childNode.m_info.m_globalPosition.x - receiverNode.m_info.m_globalPosition.x, childNode.m_info.m_globalPosition.z - receiverNode.m_info.m_globalPosition.z);
	float offsetRadii = (childNode.m_info.m_thickness + receiverNode.m_info.m_thickness);
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
					if (skeleton.RefNode(childHandle).m_info.m_thickness > maxRadius) {
						maxRadius = skeleton.RefNode(childHandle).m_info.m_thickness;
						maxRadiusNodeHandle = childHandle;
					}
				}
				if (maxRadiusNodeHandle != childNode.GetHandle()) {
					calculatedPosition += offsetVector;
				}
			}
			StrandParticle newParentParticle(particle.getIndex(), calculatedPosition, receiverNode.GetHandle());
			nodeToParticlesMap[receiverNode.GetHandle()].push_back(newParentParticle);
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

void StrandManager::resolvePbd(ShootSkeleton& skeleton, NodeHandle handle, std::vector<int> triangleIndices){
	/*float boundary = skeleton.RefNode(handle).m_info.m_thickness * 1.5f;
	float particleCount = nodeToParticlesMap.size();
	float theta = 360.0 / particleCount;
	float desiredDistance = sqrt(2 * boundary * boundary * (1 - cos(theta * 0.01745)));

	for (int i = 0; i < triangleIndices.size(); ++i) {
		//find forward edge pair
		int edgePairIndex = (i / 3) * 3 + (i + 1) % 3;
		
		StrandParticle vert = nodeToParticlesMap[handle][i];
		StrandParticle nextVert = nodeToParticlesMap[handle][edgePairIndex];

		float constraintMatched = glm::distance(vert.getLocalPosition(), nextVert.getLocalPosition()) - desiredDistance;
		glm::vec2 constraint1 = (nextVert.getLocalPosition() - vert.getLocalPosition()) / glm::distance(vert.getLocalPosition(), nextVert.getLocalPosition());

	}

	*/
	/*
	for (auto& particle : pair.second) {
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
	*/
}

std::vector<glm::vec3> StrandManager::getGlobalNodeParticlePositions(NodeHandle handle, ShootSkeleton& skeleton)
{
	std::vector<glm::vec3> particlePositions;
	for (auto& particle : nodeToParticlesMap[handle]) {
		glm::vec2 localPosition = particle.getLocalPosition();
		glm::vec3 globalPosition = skeleton.RefNode(handle).m_info.m_globalPosition;

		glm::vec3 addLocalPosition = glm::vec3(localPosition.x, 0, localPosition.y);
		
		if (skeleton.RefNode(handle).GetParentHandle() >= 0) {
			glm::vec3 parentVector = skeleton.RefNode(handle).m_info.m_globalPosition - skeleton.RefNode(skeleton.RefNode(handle).GetParentHandle()).m_info.m_globalPosition;
			addLocalPosition = glm::rotation(glm::vec3(0, 1, 0), glm::normalize(parentVector)) * addLocalPosition;
		}
		globalPosition += addLocalPosition;

		particlePositions.push_back(globalPosition);
	}

	return particlePositions;
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
	for (auto& point : nodeToParticlesMap[handle]) {
		points.push_back(point.getLocalPosition());
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
	// if we didn't cull any triangles, then we have our base delaunay mesh as the output
	// so we can just go around its convex hull
	if (true){//validHEs.size() == d.triangles.size()) {
		for (const auto& tri : d.triangles) {
			IDXs.push_back((int)tri + indexOffset);
		}
		
		// if we want to change this back to return points on a convex hull,
		// this is how:

		std::size_t curr = d.hull_start;
		std::vector<int> hullIdx;
		do {
			hullIdx.push_back(curr + indexOffset);
			curr = d.hull_next[curr]; // Move to next vertex
		} while (curr != d.hull_start); // Loop until we return to start

		return DelauneyData(IDXs, hullIdx);
	}
	//todo: fix below this is all fake
	// IV. Don't care about connectivity, just return all kept triangle indices
	for (int i = 0; i < validHEs.size(); ++i) {
		int curr = validHEs[i];
		IDXs.push_back(d.triangles[curr] + indexOffset);
	}
	return DelauneyData(IDXs, IDXs);

	// OLD CODE, LEAVING HERE IN CASE WE USE IT
	/*

	// TRAVERSING MESH TO FIND SEPARATED COMPONENTS

	std::unordered_set<int> visited;
	std:: vector<std::unordered_set<int>> components;

	for (int he : validHEs) {
		if (visited.count(he)) continue;

		// Start new component
		std::stack<int> stack;
		std::unordered_set<int> component;
		stack.push(he);

		while (!stack.empty()) {
			int curr = stack.top();
			stack.pop();
			component.insert(curr);

			// if we haven't visited this yet
			if (!visited.count(curr)) {
				visited.insert(curr);
				int next = nextHalfedge(curr);
				bool n = validHEs.count(next) && !visited.count(next);
				if (n) {
					stack.push(next);
				}
				int prev = prevHalfedge(curr);
				bool p = validHEs.count(prev) && !visited.count(prev);
				if (p) {
					stack.push(prev);
				}
				int sym = d.halfedges[curr];
				bool s = validHEs.count(sym) && !visited.count(sym);
				if (s) {
					stack.push(sym);
				}
			}
		}

		if (!component.empty()) {
			components.push_back(component);
		}
	}
	*/
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
			std::vector<int> parentLoop = nodeToDelauneyData[handle].hullIdx;
			std::vector<int> childLoop = nodeToDelauneyData[node.RefChildHandles()[0]].hullIdx;

			//convert to polar coordinates, starting at pointer to each closest to 0 degrees.
			int parentCurr = 0;
			int childCurr = 0;

			int firstParent = parentCurr;
			int firstChild = childCurr;

			bool freezeParent = false;
			bool freezeChild = false;

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

			break;
		}
		default: {
			break;
		}
		}
	}
	return bridgeTriangleIndices;
}

