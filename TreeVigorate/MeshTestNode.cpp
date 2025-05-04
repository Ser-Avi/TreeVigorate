#include "MeshTestNode.h"

//#include "delaunator.hpp"

MObject MeshTestNode::outputMesh;
MTypeId MeshTestNode::id(0x80500);

MObject MeshTestNode::maxLen;
MObject MeshTestNode::numPts;

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

	returnStatus = attributeAffects(MeshTestNode::numPts,
		MeshTestNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	returnStatus = attributeAffects(MeshTestNode::maxLen,
		MeshTestNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	return MS::kSuccess;
}

// Old mesh creation code,
// has some stuff for side faces kinda that
// we can look at to reuse
void createMesh(std::vector<glm::vec2>& pts, MPointArray& points, MIntArray& faceCounts, MIntArray& faceConns) {
	int numPs = pts.size();
	for (int i = 0; i < numPs; ++i) {
		glm::vec2 p = pts[i];
		points.append(MPoint(p.x, 0, p.y));
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
	faceCounts.append(numPs);
	for (int i = 0; i < numPs; ++i) {
			faceConns.append(i);
	}
}

MStatus MeshTestNode::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus returnStatus;

	if (plug == outputMesh) {
		// Get vars
		double mLen = data.inputValue(maxLen).asDouble();
		int numPs = data.inputValue(numPts).asInt();

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

		std::vector<int> keptTris = getPlaneTriangleIdx(pts, mLen);

		MGlobal::displayInfo("Mesh generated");

		createTris(keptTris, pts, points, faceCounts, faceConns);
		
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

void MeshTestNode::createTris(std::vector<int>& tris, std::vector<glm::vec2>& pts,
	MPointArray& points, MIntArray& faceCounts, MIntArray& faceConns) {
	for (int i = 0; i < pts.size(); ++i) {
		double x = pts[i].x;
		double z = pts[i].y;
		MPoint p(x, 0, z);
		points.append(p);
	}
	for (int i = 0; i < tris.size(); i += 3) {
		faceCounts.append(3);
		faceConns.append(tris[i]);
		faceConns.append(tris[i + 1]);
		faceConns.append(tris[i + 2]);
	}
	//int ps = points.length();
	//int cts = faceCounts.length();
	//int cons = faceConns.length();
	//MGlobal::displayInfo("debug check");
}

std::vector<int> MeshTestNode::getPlaneTriangleIdx(std::vector<glm::vec2> points, float maxEdge) {
	std::vector<int> IDXs;
	return IDXs;
	/*
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
	if (validHEs.size() == d.triangles.size()) {
		for (const auto& tri : d.triangles) {
			IDXs.push_back((int)tri);
		}
		return IDXs;

		// if we want to change this back to return points on a convex hull,
		// this is how:

		// this is if we are returning the vec2s of the hull
		//std::size_t curr = d.hull_start;
		//std::vector<glm::vec2> ps;
		//do {
		//	glm::vec2 p(d.coords[2 * curr], d.coords[2 * curr + 1]);
		//	ps.push_back(p);
		//	curr = d.hull_next[curr]; // Move to next vertex
		//} while (curr != d.hull_start); // Loop until we return to start
		//finPts.push_back(ps);
	}

	// IV. Don't care about connectivity, just return all kept triangle indices
	for (int i = 0; i < validHEs.size(); ++i) {
		int curr = validHEs[i];
		IDXs.push_back(d.triangles[curr]);
	}
	return IDXs;

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