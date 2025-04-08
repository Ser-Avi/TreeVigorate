#include "MeshTestNode.h"

#include "delaunator.hpp"

MObject MeshTestNode::outputMesh;
MTypeId MeshTestNode::id(0x80500);

MObject MeshTestNode::maxLen;
MObject MeshTestNode::numPts;
MObject MeshTestNode::alpha;

void* MeshTestNode::creator()
{
	return new MeshTestNode;
}

MStatus MeshTestNode::initialize()
{
	MFnUnitAttribute unitAttr;
	MFnTypedAttribute typedAttr;
	MFnNumericAttribute numAttr;

	MStatus returnStatus;

	MeshTestNode::maxLen = numAttr.create("maxLen", "ml", MFnNumericData::kDouble, 10.0, &returnStatus);
	McheckErr(returnStatus, "Error creating maxLen attribute \n");

	MeshTestNode::numPts = numAttr.create("numPts", "np", MFnNumericData::kInt, 5, &returnStatus);
	McheckErr(returnStatus, "Error creating numPts attribute \n");

	MeshTestNode::alpha = numAttr.create("alpha", "a", MFnNumericData::kDouble, 0.5, &returnStatus);
	McheckErr(returnStatus, "Error creating alpha attribute \n");

	MeshTestNode::outputMesh = typedAttr.create("outputMesh", "out",
		MFnData::kMesh,
		MObject::kNullObj,
		&returnStatus);
	McheckErr(returnStatus, "ERROR creating TreeNode output attribute\n");
	/*typedAttr.setStorable(false);*/

	returnStatus = addAttribute(MeshTestNode::outputMesh);
	McheckErr(returnStatus, "ERROR adding outputMesh attribute\n");

	returnStatus = addAttribute(MeshTestNode::maxLen);
	McheckErr(returnStatus, "ERROR adding maxlen attribute\n");

	returnStatus = addAttribute(MeshTestNode::numPts);
	McheckErr(returnStatus, "ERROR adding numpts attribute\n");

	returnStatus = addAttribute(MeshTestNode::alpha);
	McheckErr(returnStatus, "ERROR adding numpts attribute\n");

	returnStatus = attributeAffects(MeshTestNode::numPts,
		MeshTestNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	returnStatus = attributeAffects(MeshTestNode::alpha,
		MeshTestNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	returnStatus = attributeAffects(MeshTestNode::maxLen,
		MeshTestNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	return MS::kSuccess;
}

void createMesh(std::vector<glm::vec2>& pts, MPointArray& points, MIntArray& faceCounts, MIntArray& faceConns) {
	int numPs = pts.size();
	for (int i = 0; i < numPs; ++i) {
		glm::vec2 p = pts[i];
		points.append(MPoint(p.x, 0, p.y));
		//points.append(MPoint(p.x, 5, p.y));
	}
	//for (int i = 0; i < numPs; ++i) {
	//	int next = (i + 1) % numPs;
	//	faceCounts.append(4);
	//	// Front vertices are even indices
	//	// Back vertices are odd indices
	//	faceConns.append(2 * i);
	//	faceConns.append(2 * next);
	//	faceConns.append(2 * next + 1);
	//	faceConns.append(2 * i + 1);
	//}
	// Add front and back faces
	faceCounts.append(numPs);
	//faceCounts.append(numPs);
	//numPs *= 2;
	for (int i = 0; i < numPs; ++i) {
		//if (i < numPs * 0.5) {
			faceConns.append(i);
		//}
		//else {
			// when numPs 8.
			// i = 4 -> numPs - 1 -> 7
			// i = 5 -> numPs - 3 -> 5
			// i = 6 -> numPs - 5 -> 3
			// i = 7 -> numPs - 7 -> 1
			//faceConns.append(numPs - ((i * 2) % numPs) - 1);
		//}
	}

	// Add back face (reverse order for correct normal)
	//faceCounts.append(numPs);
	//for (int i = numPs - 1; i >= 0; --i) {
	//	faceConns.append(2 * i); // Back vertices in reverse order
	//}
}

void createTris(std::vector<std::vector<double>>& tris, MPointArray& points, MIntArray& faceCounts, MIntArray& faceConns) {
	for (int i = 0; i < tris[0].size(); i += 2) {
		double x = tris[0][i];
		double z = tris[0][i + 1];
		MPoint p(x, 0, z);
		points.append(p);
	}
	for (int i = 0; i < tris[1].size(); i += 3) {
		faceCounts.append(3);
		faceConns.append(tris[1][i]);
		faceConns.append(tris[1][i + 1]);
		faceConns.append(tris[1][i + 2]);

		if (tris[1][i] > points.length() - 1 ||
			tris[1][i + 1] > points.length() - 1 ||
			tris[1][i + 2] > points.length() - 1) {

			MGlobal::displayInfo("ruh roh");
		}
	}
	int ps = points.length();
	int cts = faceCounts.length();
	int cons = faceConns.length();
	MGlobal::displayInfo("debug check");
}

MStatus MeshTestNode::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus returnStatus;

	if (plug == outputMesh) {
		// Get vars
		double mLen = data.inputValue(maxLen).asDouble();
		int numPs = data.inputValue(numPts).asInt();
		double alp = data.inputValue(alpha).asDouble();

		/*MString tester = ("Angle Test: " + std::to_string(fAngle)).c_str();
		MGlobal::displayInfo(tester);
		tester = ("Step Test: " + std::to_string(fStep)).c_str();
		MGlobal::displayInfo(tester);
		tester = ("Grammar Test: " + g);
		MGlobal::displayInfo(tester);
		tester = ("Time Test: " + std::to_string(iterNum)).c_str();
		MGlobal::displayInfo(tester);*/


		// create output object
		MDataHandle outputHandle = data.outputValue(outputMesh, &returnStatus);
		McheckErr(returnStatus, "ERROR getting polygon data handle\n");

		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);
		McheckErr(returnStatus, "ERROR creating outputData");

		MPointArray points;
		MIntArray faceCounts;
		MIntArray faceConns;

		std::vector<glm::vec2> pts;

		float theta = 3.1415926535 * 2.f / (float)numPs;
		float rad = 5;
		for (int i = 0; i < numPs; ++i) {
			//float angle = theta * i;
			//pts.push_back(glm::vec2(cos(angle), sin(angle)) * rad);
			pts.push_back(glm::diskRand(rad));
			/*if (i % 2 == 0) {
				pts.push_back(glm::vec2(-10 + i * 0.2, i * 0.5));
			}
			else {
				pts.push_back(glm::vec2(10 + i * 0.2, i * 0.5));
			}*/
		}

		//pts.push_back(glm::vec2(0, 0));
		//pts.push_back(glm::vec2(1, 0));
		//pts.push_back(glm::vec2(1, 1));
		//pts.push_back(glm::vec2(0, 1));

		//pts.push_back(glm::vec2(10, 0));
		//pts.push_back(glm::vec2(11, 0));
		//pts.push_back(glm::vec2(11, 1));
		//pts.push_back(glm::vec2(10, 1));
		
		//createMesh(pts, points, faceCounts, faceConns);

		//std::vector<std::vector<glm::vec2>> boundaryPoints = getBoundaryPts(pts, mLen);

		std::vector<std::vector<double>> keptTris = getBoundaryPts(pts, mLen);

		MGlobal::displayInfo("Mesh generated");

		createTris(keptTris, points, faceCounts, faceConns);
		
		//for (int i = 0; i < boundaryPoints.size(); ++i) {
			//createMesh(boundaryPoints[i], points, faceCounts, faceConns);
		//}

		//createMesh(pts, points, faceCounts, faceConns);

		MFnMesh mesh;
		mesh.create(points.length(), faceCounts.length(), points, faceCounts, faceConns, newOutputData, &returnStatus);
		McheckErr(returnStatus, "ERROR creating new Mesh");

		outputHandle.set(newOutputData);
		data.setClean(plug);
	}
	else
		return MS::kUnknownParameter;

	return MS::kSuccess;
}

std::size_t nextHalfedge(std::size_t i) {
	return (i % 3 == 2) ? i - 2 : i + 1;
}
std::size_t prevHalfedge(std::size_t i) {
	return (i % 3 == 0) ? i + 2 : i - 1;
}

bool noSym(std::size_t i, const std::vector<size_t>& HEs) {
	return HEs[i] < 0 || HEs[i] > HEs.size() + 1;
}

std::vector<std::vector<double>> MeshTestNode::getBoundaryPts(std::vector<glm::vec2> points, float maxEdge) {
	std::vector<std::vector<double>> finPts;

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
	// we will store the valid HEs in a map, with their number of appearances
	// 2 means they are an inner HE, 1 means they are on the boundary
	std::vector<int> validHEs;
	// we will store the invalid HEs, to know when we have hit a HE that we cull
	std::unordered_set<int> invalidHEs;
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

		if (glm::length(e01) > maxEdge || glm::length(e12) > maxEdge || glm::length(e20) > maxEdge) {
			invalidHEs.insert(i);
			invalidHEs.insert(i + 1);
			invalidHEs.insert(i + 2);
		}
		else {
			//validHEs[d.triangles[i]]++;
			//validHEs[d.triangles[i + 1]]++;
			//validHEs[d.triangles[i + 2]]++;
			validHEs.push_back(i);
			validHEs.push_back(i + 1);
			validHEs.push_back(i + 2);
		}
	}

	// if we didn't cull any triangles, then we have our base delaunay mesh as the output
	// so we can just go around its convex hull
	if (invalidHEs.size() == 0) {
		finPts.push_back(d.coords);
		std::vector<double> tris;
		for (const auto& tri : d.triangles) {
			tris.push_back((double)tri);
		}
		finPts.push_back(tris);
		return finPts;
		// this is if we are returning the vec2s of the hull
		//std::size_t curr = d.hull_start;
		//std::vector<glm::vec2> ps;
		//do {
		//	glm::vec2 p(d.coords[2 * curr], d.coords[2 * curr + 1]);
		//	ps.push_back(p);
		//	curr = d.hull_next[curr]; // Move to next vertex
		//} while (curr != d.hull_start); // Loop until we return to start
		//finPts.push_back(ps);
		//return finPts;
	}

	// IV. A. don't care about connectivity, just draw all kept triangles
	// finpts will have
	
	// since we're keeping all triangles, we can just push back all the vert coords and keep them
	finPts.push_back(d.coords);
	std::vector<double> idx;

	for (int i = 0; i < validHEs.size(); ++i) {
		int curr = validHEs[i];
		idx.push_back(d.triangles[curr]);
	}
	finPts.push_back(idx);
	return finPts;

	// IV. B. we get each component and its half edges
	/*
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

	//// now we just redelaunay and add the points
	//for (const auto& component : components) {
	//	// Step 1: get unique verts
	//	std::unordered_set<size_t> uniqueVerts;
	//	for (size_t he : component) {
	//		uniqueVerts.insert(d.triangles[he]);
	//		uniqueVerts.insert(d.triangles[nextHalfedge(he)]);
	//	}
	//	// Step 2: build new coords
	//	std::vector<double> newCoords;
	//	for (size_t v : uniqueVerts) {
	//		newCoords.push_back(d.coords[2 * v]);
	//		newCoords.push_back(d.coords[2 * v + 1]);
	//	}
	//	// Step 3: build new mesh
	//	delaunator::Delaunator newMesh(newCoords);
	//	// Step 4: add the points to the final points
	//	std::vector<glm::vec2> segPs;
	//	//int curr = newMesh.hull_start;
	//	//do {
	//	//	int v = newMesh.triangles[curr];	// current vertex
	//	//	double x = newMesh.coords[2 * v], y = newMesh.coords[2 * v + 1];
	//	//	segPs.push_back(glm::vec2(x, y));

	//	//	// we will then loop around this vertex with sym->next until we find an edge that doesn't have a sym-> is on the border
	//	//	bool isBoundary = false;
	//	//	while (!isBoundary) {
	//	//		curr = nextHalfedge(curr);
	//	//		if (noSym(curr, newMesh.halfedges)) {
	//	//			isBoundary = true;
	//	//		}
	//	//		else {
	//	//			curr = newMesh.halfedges[curr];
	//	//		}
	//	//	}
	//	//} while (curr != newMesh.hull_start);
	//	std::unordered_set<int> visited_edges;

	//	// Find first boundary edge (convex hull or interior hole)
	//	int start = -1;
	//	for (size_t i = 0; i < newMesh.halfedges.size(); ++i) {
	//		if (newMesh.halfedges[i] == delaunator::INVALID_INDEX) {
	//			start = i;
	//			break;
	//		}
	//	}

	//	if (start == -1) continue; // No boundaries found

	//	// Traverse the boundary loop
	//	int current = start;
	//	do {
	//		// Add current vertex
	//		int v = newMesh.triangles[current];
	//		segPs.push_back(glm::vec2(
	//			newMesh.coords[2 * v],
	//			newMesh.coords[2 * v + 1]
	//		));
	//		visited_edges.insert(current);

	//		// Find next boundary edge
	//		current = nextHalfedge(current);
	//		while (newMesh.halfedges[current] != delaunator::INVALID_INDEX) {
	//			current = newMesh.halfedges[current]; // Jump to opposite triangle
	//			current = prevHalfedge(current);   // Move to previous half-edge
	//		}
	//	} while (current != start && visited_edges.count(current) == 0);

	//	finPts.push_back(segPs);
	//}


	// IV. Next we split points if they are disconnected
	// we find connected components by traversing through all points,
	// marking them as visited until we run out of points
	// this vector of vector tracks adjacent half edges

	/*
	std::vector<std::vector<int>> HEAdj;
	std::vector<int> boundaryEdges;
	std::unordered_set<int> visited;
	//int curr = validHEs.begin()->first;
	std::vector<std::vector<int>> separatedHEs;
	int segment = 0;
	for (int i = 0; i < validHEs.size(); ++i) {
		if (visited.count(i)) continue; // if visited continue
		int sym = d.halfedges[i];
		// if sym is a valid half edge, insert it in the right segment too
		if (validHEs.find(sym) != validHEs.end()) {
			visited.insert(sym);
			separatedHEs[segment].push_back(sym);
		}
		std::size_t from = d.triangles[i];
		std::size_t to = d.triangles[nextHalfedge(i)]; // Next half-edge in triangle
		visited.insert(i);
		separatedHEs[segment].push_back(i);
	}*/


	//finPts.push_back(points);
	return finPts;
}