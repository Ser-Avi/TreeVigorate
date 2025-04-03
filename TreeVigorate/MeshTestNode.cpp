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
		points.append(MPoint(p.x, 5, p.y));
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
	faceCounts.append(numPs);
	numPs *= 2;
	for (int i = 0; i < numPs; ++i) {
		if (i < numPs * 0.5) {
			faceConns.append(i * 2);
		}
		else {
			// when numPs 8.
			// i = 4 -> numPs - 1 -> 7
			// i = 5 -> numPs - 3 -> 5
			// i = 6 -> numPs - 5 -> 3
			// i = 7 -> numPs - 7 -> 1
			faceConns.append(numPs - ((i * 2) % numPs) - 1);
		}
	}

	// Add back face (reverse order for correct normal)
	//faceCounts.append(numPs);
	//for (int i = numPs - 1; i >= 0; --i) {
	//	faceConns.append(2 * i); // Back vertices in reverse order
	//}
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
			//pts.push_back(glm::diskRand(rad));
			/*if (i % 2 == 0) {
				pts.push_back(glm::vec2(-10 + i * 0.2, i * 0.5));
			}
			else {
				pts.push_back(glm::vec2(10 + i * 0.2, i * 0.5));
			}*/
		}

		pts.push_back(glm::vec2(0, 0));
		pts.push_back(glm::vec2(1, 0));
		pts.push_back(glm::vec2(1, 1));
		pts.push_back(glm::vec2(0, 1));

		pts.push_back(glm::vec2(10, 0));
		pts.push_back(glm::vec2(11, 0));
		pts.push_back(glm::vec2(11, 1));
		pts.push_back(glm::vec2(10, 1));
		
		//createMesh(pts, points, faceCounts, faceConns);

		std::vector<std::vector<glm::vec2>> boundaryPoints = getBoundaryPts(pts, mLen);

		MGlobal::displayInfo("Mesh generated");

		for (int i = 0; i < boundaryPoints.size(); ++i) {
			createMesh(boundaryPoints[i], points, faceCounts, faceConns);
		}

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

std::vector<std::vector<glm::vec2>> MeshTestNode::getBoundaryPts(std::vector<glm::vec2> points, float maxEdge) {
	std::vector<std::vector<glm::vec2>> finPts;
	finPts.push_back(points);
	return finPts;
}