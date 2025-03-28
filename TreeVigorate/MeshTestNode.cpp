#include "MeshTestNode.h"

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
		points.append(MPoint(p.x, 1, p.y));
	}
	for (int i = 0; i < numPs; ++i) {
		int next = (i + 1) % numPs;
		faceCounts.append(4);
		// Front vertices are even indices
		// Back vertices are odd indices
		faceConns.append(2 * i);
		faceConns.append(2 * next);
		faceConns.append(2 * next + 1);
		faceConns.append(2 * i + 1);
	}
	// Add front face (original polygon)
	faceCounts.append(numPs);
	for (int i = 0; i < numPs; ++i) {
		faceConns.append(2 * i); // Front vertices in order
	}

	// Add back face (reverse order for correct normal)
	faceCounts.append(numPs);
	for (int i = numPs - 1; i >= 0; --i) {
		faceConns.append(2 * i + 1); // Back vertices in reverse order
	}
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

		//float theta = 3.1415926535 * 2.f / (float)numPs;
		float rad = 5;
		for (int i = 0; i < numPs; ++i) {
			//float angle = theta * i;
			//pts.push_back(glm::vec2(cos(angle), sin(angle)) * rad);
			pts.push_back(glm::diskRand(rad));
		}

		// delaunay triangulate points
		Delaunay dt;
		std::vector<CGAL_Point> cgalPts = convertToCGAL(pts);
		dt.insert(cgalPts.begin(), cgalPts.end());

		// removing long edges
		std::vector<std::pair<CGAL_Point, CGAL_Point>> edges;
		for (auto edge = dt.finite_edges_begin(); edge != dt.finite_edges_end(); ++edge) {
			auto seg = dt.segment(edge); // Get edge as a segment
			if (CGAL::sqrt(seg.squared_length()) <= mLen) {
				edges.emplace_back(seg.source(), seg.target());
			}
			else {
				MGlobal::displayInfo("Edge Cut");
			}
		}

		// getting bounding edges
		Alpha_shape alphaShape(dt);
		//double alpha = 0.5; // Adjust for concavity (smaller = more concave)
		//alphaShape.set_alpha(alp);
		alphaShape.set_alpha(*alphaShape.find_optimal_alpha(1));
		MGlobal::displayInfo("Alpha shape generated");

		// clearing vectors that we'll reuse here
		cgalPts.clear();
		pts.clear();

		//std::vector<Alpha_shape::Edge> boundaryEdges;
		for (auto edge = alphaShape.alpha_shape_edges_begin();
			edge != alphaShape.alpha_shape_edges_end();
			++edge) {
			if (alphaShape.classify(*edge) == Alpha_shape::REGULAR) {
				//boundaryEdges.push_back(*edge);
				MGlobal::displayInfo("in if");
				
				// getting the point information
				// Get the face containing the edge
				Alpha_shape::Face_handle face = edge->first;
				int edgeIndex = edge->second; // 0, 1, or 2

				// Get the first vertex on the edge (since we know this is a loop)
				const K::Point_2& p1 = face->vertex((edgeIndex + 1) % 3)->point();
				//const K::Point_2& p2 = face->vertex((edgeIndex + 2) % 3)->point();
				//edgeVertices.emplace_back(p1, p2);
				cgalPts.push_back(p1);
			}
			else {
				MGlobal::displayInfo("failed");
			}
		}

		MGlobal::displayInfo("Mesh generated");

		pts = convertToGLM(cgalPts);

		createMesh(pts, points, faceCounts, faceConns);

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

std::vector<CGAL_Point> MeshTestNode::convertToCGAL(std::vector<glm::vec2>& pts) {
	std::vector<CGAL_Point> cgalPs;
	for (const glm::vec2& p : pts) {
		cgalPs.emplace_back(p.x, p.y);
	}
	return cgalPs;
}
std::vector<glm::vec2> MeshTestNode::convertToGLM(std::vector<CGAL_Point>& pts) {
	std::vector<glm::vec2> glmPs;
	for (const CGAL_Point& p : pts) {
		glmPs.emplace_back(glm::vec2(p[0], p[1]));
	}
	return glmPs;
}