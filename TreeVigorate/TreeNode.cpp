#include "TreeNode.h"

MObject TreeNode::outputMesh;
MTypeId TreeNode::id(0x80000);

MObject TreeNode::deltaTime;
//MObject TreeNode::stepSize;
//MObject TreeNode::grammar;

void* TreeNode::creator()
{
	return new TreeNode;
}

MStatus TreeNode::initialize()
{
	MFnUnitAttribute unitAttr;
	MFnTypedAttribute typedAttr;
	MFnNumericAttribute numAttr;

	MStatus returnStatus;

	TreeNode::deltaTime = numAttr.create("deltaTime", "dt", MFnNumericData::kDouble, 0.0, &returnStatus);
	McheckErr(returnStatus, "Error creating TreeNode deltaTime attribute \n");

	//TreeNode::stepSize = numAttr.create("stepSize", "s", MFnNumericData::kDouble, 0, &returnStatus);
	//McheckErr(returnStatus, "Error creating TreeNode step size attribute \n");

	//TreeNode::grammar = typedAttr.create("grammar", "g", MFnData::kString, MObject::kNullObj, &returnStatus);
	//McheckErr(returnStatus, "Error creating TreeNode grammar attribute \n");

	//TreeNode::time = unitAttr.create("time", "tm",
	//	MFnUnitAttribute::kTime,
	//	0.0, &returnStatus);
	//McheckErr(returnStatus, "ERROR creating TreeNode time attribute\n");


	TreeNode::outputMesh = typedAttr.create("outputMesh", "out",
		MFnData::kMesh,
		MObject::kNullObj,
		&returnStatus);
	McheckErr(returnStatus, "ERROR creating TreeNode output attribute\n");
	/*typedAttr.setStorable(false);*/

	//returnStatus = addAttribute(TreeNode::time);
	//McheckErr(returnStatus, "ERROR adding time attribute\n");

	returnStatus = addAttribute(TreeNode::outputMesh);
	McheckErr(returnStatus, "ERROR adding outputMesh attribute\n");

	returnStatus = addAttribute(TreeNode::deltaTime);
	McheckErr(returnStatus, "ERROR adding deltaTime attribute\n");

	//returnStatus = addAttribute(TreeNode::grammar);
	//McheckErr(returnStatus, "ERROR adding grammar attribute\n");

	//returnStatus = addAttribute(TreeNode::stepSize);
	//McheckErr(returnStatus, "ERROR adding stepSize attribute\n");

	//returnStatus = attributeAffects(TreeNode::time,
	//	TreeNode::outputMesh);
	//McheckErr(returnStatus, "ERROR in attributeAffects\n");

	returnStatus = attributeAffects(TreeNode::deltaTime,
		TreeNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");
	//returnStatus = attributeAffects(TreeNode::stepSize,
	//	TreeNode::outputMesh);
	//McheckErr(returnStatus, "ERROR in attributeAffects\n");
	//returnStatus = attributeAffects(TreeNode::grammar,
	//	TreeNode::outputMesh);
	//McheckErr(returnStatus, "ERROR in attributeAffects\n");

	return MS::kSuccess;
}

MStatus TreeNode::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus returnStatus;

	if (plug == outputMesh) {
		// Get vars
		double fDTime = data.inputValue(deltaTime).asDouble();
		//double fStep = data.inputValue(stepSize).asDouble();
		//MString g = data.inputValue(grammar).asString();
		/* Get time */
		//MDataHandle timeData = data.inputValue(time, &returnStatus);
		//McheckErr(returnStatus, "Error getting time data handle\n");
		//MTime time = timeData.asTime();
		//const int iterNum = (int)time.as(MTime::kFilm);

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


		MPoint start(0.0, 0.0, 0.0);
		MPoint end(0.3, 1, 0.2);
		CylinderMesh curr(start, end);
		curr.appendToMesh(points, faceCounts, faceConns);

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