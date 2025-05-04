#include "NodeNode.h"

MObject NodeNode::outputMesh;
MTypeId NodeNode::id(0x80002);

MObject NodeNode::pos;
MObject NodeNode::nodeID;
MObject NodeNode::vigor;
MObject NodeNode::rot;
MObject NodeNode::isVisible;

void* NodeNode::creator()
{
	return new NodeNode;
}

MStatus NodeNode::initialize()
{
	MFnUnitAttribute unitAttr;
	MFnTypedAttribute typedAttr;
	MFnNumericAttribute numAttr;

	MStatus returnStatus;

	NodeNode::pos = numAttr.create("pos", "p", MFnNumericData::k3Double, 0, &returnStatus);
	numAttr.setNiceNameOverride("World Position");
	numAttr.setDefault(0.0, 0.0, 0.0);
	McheckErr(returnStatus, "Error creating world pos attribute \n");

	NodeNode::nodeID = numAttr.create("nodeID", "ni", MFnNumericData::kInt, 0, &returnStatus);
	McheckErr(returnStatus, "Error creating nodeID attribute \n");

	NodeNode::vigor = numAttr.create("vigor", "v", MFnNumericData::kDouble, 1.0, &returnStatus);
	McheckErr(returnStatus, "Error creating vigor attribute \n");

	NodeNode::rot = numAttr.create("rot", "r", MFnNumericData::kDouble, 0.0, &returnStatus);
	McheckErr(returnStatus, "Error creating rotation attribute \n");

	NodeNode::isVisible = numAttr.create("isVisible", "iv", MFnNumericData::kBoolean, true, &returnStatus);
	McheckErr(returnStatus, "Error creating makeGrow attribute\n");

	NodeNode::outputMesh = typedAttr.create("outputMesh", "out",
		MFnData::kMesh,
		MObject::kNullObj,
		&returnStatus);
	McheckErr(returnStatus, "ERROR creating NodeNode output attribute\n");
	typedAttr.setHidden(true);

	returnStatus = addAttribute(NodeNode::pos);
	McheckErr(returnStatus, "ERROR adding pos attribute\n");

	returnStatus = addAttribute(NodeNode::nodeID);
	McheckErr(returnStatus, "ERROR adding nodeID attribute\n");

	returnStatus = addAttribute(NodeNode::vigor);
	McheckErr(returnStatus, "ERROR adding vigor attribute\n");

	returnStatus = addAttribute(NodeNode::rot);
	McheckErr(returnStatus, "ERROR adding rot attribute\n");

	returnStatus = addAttribute(NodeNode::isVisible);
	McheckErr(returnStatus, "ERROR adding isVisible attribute\n");

	returnStatus = addAttribute(NodeNode::outputMesh);
	McheckErr(returnStatus, "ERROR adding outmesh attribute\n");

	returnStatus = attributeAffects(NodeNode::isVisible,
		NodeNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	returnStatus = attributeAffects(NodeNode::pos,
		NodeNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	return MS::kSuccess;
}

MStatus NodeNode::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus returnStatus;

	if (plug == outputMesh) {
		// create output
		MDataHandle outputHandle = data.outputValue(outputMesh, &returnStatus);
		McheckErr(returnStatus, "ERROR getting polygon data handle\n");

		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);
		McheckErr(returnStatus, "ERROR creating outputData");

		MPointArray points;
		MIntArray faceCounts;
		MIntArray faceConns;
		
		// Get vars
		bool isVis = data.inputValue(isVisible).asBool();
			// if we're not visible, just return an empty mesh
		//if (!isVis) {
		//	MFnMesh mesh;
		//	mesh.create(points.length(), faceCounts.length(), points, faceCounts, faceConns, newOutputData, &returnStatus);
		//	McheckErr(returnStatus, "ERROR creating new Mesh");
		//	outputHandle.set(newOutputData);
		//	data.setClean(plug);
		//	return MS::kSuccess;
		//}
		
		double3& position = data.inputValue(pos).asDouble3();
		glm::vec3 posVec = glm::vec3(position[0], position[1], position[2]);
		int ID = data.inputValue(nodeID).asInt();
		float vig = data.inputValue(vigor).asDouble();
		float rotation = data.inputValue(rot).asDouble();

		createCube(posVec, vig, points, faceCounts, faceConns);

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

void NodeNode::createCube(glm::vec3 pos, float size, MPointArray& points, MIntArray& faceCounts, MIntArray& faceConnects) {
	MPoint mPos(pos.x, pos.y, pos.z);
	points.append(MPoint(-size, -size, -size) + mPos);			// back bottom left
	points.append(MPoint(+size, -size, -size) + mPos);			// front bottom left
	points.append(MPoint(+size, -size, +size) + mPos);			// front bottom right
	points.append(MPoint(-size, -size, +size) + mPos);			// back bottom right
	points.append(MPoint(-size, +size, -size) + mPos);			// back top left
	points.append(MPoint(-size, +size, +size) + mPos);			// back top right
	points.append(MPoint(+size, +size, +size) + mPos);			// front top right
	points.append(MPoint(+size, +size, -size) + mPos);			// front top left

	for (int i = 0; i < 6; ++i) {
		faceCounts.append(4);
	}

	int fc[24] = {0, 1, 2, 3, 4, 5, 6, 7, 3, 2, 6, 5, 0, 3, 5, 4, 0, 4, 7, 1, 1, 7, 6, 2};
	for (int i = 0; i < 24; ++i) {
		faceConnects.append(fc[i]);
	}
	return;
}