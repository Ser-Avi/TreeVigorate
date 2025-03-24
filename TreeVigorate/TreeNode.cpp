#include "TreeNode.h"

/*
MEL Script to initialize this with:
global proc createTreeNode() {
	createNode transform -n TSys1;
	createNode mesh -n TShape1 -p TSys1;
	sets -add initialShadingGroup TShape1;
	createNode TreeNode -n TN1;
	connectAttr TN1.outputMesh TShape1.inMesh;
};
createTreeNode
*/

MObject TreeNode::outputMesh;
MTypeId TreeNode::id(0x80000);

MObject TreeNode::deltaTime;
MObject TreeNode::numGrows;
MObject TreeNode::radius;
MObject TreeNode::treeDataFile;
MObject TreeNode::makeGrow;

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

	TreeNode::deltaTime = numAttr.create("deltaTime", "dt", MFnNumericData::kDouble, 0.1, &returnStatus);
	McheckErr(returnStatus, "Error creating TreeNode deltaTime attribute \n");

	TreeNode::numGrows = numAttr.create("numGrows", "ng", MFnNumericData::kInt, 0, &returnStatus);
	McheckErr(returnStatus, "Error creating TreeNode numGrows attribute \n");

	TreeNode::radius = numAttr.create("radius", "r", MFnNumericData::kDouble, 1.0, &returnStatus);
	McheckErr(returnStatus, "Error creatin radius attribute \n");

	TreeNode::treeDataFile = typedAttr.create("treeDataFile", "f", MFnData::kString, MObject::kNullObj, &returnStatus);
	McheckErr(returnStatus, "Error creating treeDataFile attribute \n");

	TreeNode::makeGrow = numAttr.create("makeGrow", "mg", MFnNumericData::kBoolean, false, &returnStatus);
	McheckErr(returnStatus, "Error creating makeGrow attribute\n");

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

	returnStatus = addAttribute(TreeNode::numGrows);
	McheckErr(returnStatus, "ERROR adding numGrows attribute\n");

	returnStatus = addAttribute(TreeNode::radius);
	McheckErr(returnStatus, "ERROR adding radius attribute\n");

	returnStatus = addAttribute(TreeNode::treeDataFile);
	McheckErr(returnStatus, "ERROR adding treeDataFile attribute\n");

	returnStatus = addAttribute(TreeNode::makeGrow);
	McheckErr(returnStatus, "ERROR adding makeGrow attribute\n");

	//returnStatus = attributeAffects(TreeNode::time,
	//	TreeNode::outputMesh);
	//McheckErr(returnStatus, "ERROR in attributeAffects\n");

	returnStatus = attributeAffects(TreeNode::makeGrow,
		TreeNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	return MS::kSuccess;
}

MStatus TreeNode::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus returnStatus;

	if (plug == outputMesh) {
		// Get vars
		double fDTime = data.inputValue(deltaTime).asDouble();
		int nGrows = data.inputValue(numGrows).asInt();
		float r = data.inputValue(radius).asDouble();
		MString treeInfo = data.inputValue(treeDataFile).asString();

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


		// Initializing tree params
		SoilModel m_soilModel;
		ClimateModel m_climateModel;
		RootGrowthController m_rootGrowthParameters;
		ShootGrowthController m_shootGrowthParameters;
		InitializeMVars(treeInfo.asChar(), m_soilModel, m_climateModel, m_rootGrowthParameters, m_shootGrowthParameters);

		// Growing tree
		MGlobal::displayInfo("Abt to Grow, stand back!!");
		for (int i = 0; i < nGrows; ++i) {
			int nodes = treeModel.RefShootSkeleton().RefSortedNodeList().size();
			for (int j = 0; j < nodes; ++j) {
				auto& snode = treeModel.RefShootSkeleton().RefNode(j);
			}
			bool didGrow = treeModel.Grow(fDTime, glm::mat4(), m_soilModel, m_climateModel, m_rootGrowthParameters, m_shootGrowthParameters);
			MGlobal::displayInfo("Growth successful, iteration: ");
			MGlobal::displayInfo(std::to_string(i).c_str());
			
			MGlobal::displayInfo("Shoot nodes: ");
			MGlobal::displayInfo(MString(std::to_string(nodes).c_str()));
			
			int flows = treeModel.RefShootSkeleton().RefSortedFlowList().size();
			MGlobal::displayInfo("Flows nodes: ");
			MGlobal::displayInfo(MString(std::to_string(flows).c_str()));
		}

		MGlobal::displayInfo("Rad:");
		MGlobal::displayInfo(std::to_string(r).c_str());

		// Creating cylinders
		ShootSkeleton shoots = treeModel.RefShootSkeleton();
		if (shoots.RefSortedNodeList().size() != 0) {
			int rootHandle = shoots.RefSortedNodeList()[0];
			auto& rootNode = shoots.RefNode(rootHandle);
			bool isAdd = appendNodeCylindersToMesh(points, faceCounts, faceConns, shoots, r);
		}

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

bool TreeNode::appendNodeCylindersToMesh(MPointArray& points, MIntArray& faceCounts, MIntArray& faceConns, ShootSkeleton& skeleton, double radius) {
	for (int i = 1; i < skeleton.RefSortedNodeList().size(); ++i)
	{
		int currHandle = skeleton.RefSortedNodeList()[i];
		auto& curr = skeleton.PeekNode(currHandle);
		glm::vec3 currPos = curr.m_info.m_globalPosition;
		int parentHandle = curr.GetParentHandle();
		auto& parent = skeleton.PeekNode(parentHandle);
		glm::vec3 parentPos = parent.m_info.m_globalPosition;

		MPoint start(parentPos[0], parentPos[1], parentPos[2]);
		MPoint end(currPos[0], currPos[1], currPos[2]);
		CylinderMesh cyl(start, end, curr.m_info.m_thickness * radius);
		cyl.appendToMesh(points, faceCounts, faceConns);
	}
	return true;
}

// PARSING FILES
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


// These params don't align 1 to 1 with actual params that we use,
// so this is somehow the best method atm.
struct SGParams {
	int m_internodeGrowthRate;
	int m_leafGrowthRate;
	int m_fruitGrowthRate;
	int m_lateralBudCount;
	int m_fruitBudCount;
	int m_leafBudCount;
	float m_branchingAngleMeanVariance[2];
	float m_rollAngleMeanVariance[2];
	float m_apicalAngleMeanVariance[2];
	float m_gravitropism;
	float m_phototropism;
	float m_internodeLength;
	float m_endNodeThickness;
	float m_thicknessAccumulationFactor;
	float m_thicknessAccumulateAgeFactor;
	float m_lateralBudFlushingProbabilityTemperatureRange[4];
	float m_leafBudFlushingProbabilityTemperatureRange[4];
	float m_fruitBudFlushingProbabilityTemperatureRange[4];
	float m_apicalBudLightingFactor;
	float m_lateralBudLightingFactor;
	float m_leafBudLightingFactor;
	float m_fruitBudLightingFactor;
	float m_apicalControl;
	float m_apicalControlAgeFactor;
	float m_apicalDominance;
	float m_apicalDominanceAgeFactor;
	float m_apicalDominanceDistanceFactor;
	float m_apicalBudExtinctionRate;
	float m_lateralBudExtinctionRate;
	float m_leafBudExtinctionRate;
	float m_fruitBudExtinctionRate;
	float m_leafVigorRequirement;
	float m_fruitVigorRequirement;
	float m_internodeVigorRequirement;
	float m_vigorRequirementAggregateLoss;
	float m_lowBranchPruning;
	float m_saggingFactorThicknessReductionMax[3];
	float m_maxLeafSize[3];
	float m_leafPositionVariance;
	float m_leafRandomRotation;
	float m_leafChlorophyllLoss;
	float m_leafChlorophyllSynthesisFactorTemperature;
	float m_leafFallProbability;
	float m_leafDistanceToBranchEndLimit;
	float m_maxFruitSize[3];
	float m_fruitPositionVariance;
	float m_fruitRandomRotation;
	float m_fruitFallProbability;
};

struct RGParams {
	float m_branchingAngleMeanVariance[2];
	float m_rollAngleMeanVariance[2];
	float m_apicalAngleMeanVariance[2];
	float m_rootNodeLength;
	float m_rootNodeGrowthRate;
	float m_endNodeThickness;
	float m_thicknessAccumulationFactor;
	float m_thicknessAccumulateAgeFactor;
	float m_rootNodeVigorRequirement;
	float m_vigorRequirementAggregateLoss;
	float m_environmentalFriction;
	float m_environmentalFrictionFactor;
	float m_apicalControl;
	float m_apicalControlAgeFactor;
	float m_apicalDominance;
	float m_apicalDominanceAgeFactor;
	float m_apicalDominanceDistanceFactor;
	float m_tropismSwitchingProbability;
	float m_tropismSwitchingProbabilityDistanceFactor;
	float m_tropismIntensity;
	float m_branchingProbability;
};

struct PlantParameters {
	SGParams sg;
	RGParams rg;
};

void parseArray(std::istringstream& iss, float* array, size_t size) {
	char separator;
	iss >> separator; // Read the opening '['
	for (size_t i = 0; i < size; ++i) {
		iss >> array[i];
		if (i < size - 1) iss >> separator; // Read the comma
	}
	iss >> separator; // Read the closing ']'
}

void parseShootGrowthParameters(std::ifstream& file, SGParams& params) {
	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string key;
		iss >> key;

		if (key == "m_internodeGrowthRate:") iss >> params.m_internodeGrowthRate;
		else if (key == "m_leafGrowthRate:") iss >> params.m_leafGrowthRate;
		else if (key == "m_fruitGrowthRate:") iss >> params.m_fruitGrowthRate;
		else if (key == "m_lateralBudCount:") iss >> params.m_lateralBudCount;
		else if (key == "m_fruitBudCount:") iss >> params.m_fruitBudCount;
		else if (key == "m_leafBudCount:") iss >> params.m_leafBudCount;
		else if (key == "m_branchingAngleMeanVariance:") parseArray(iss, params.m_branchingAngleMeanVariance, 2);
		else if (key == "m_rollAngleMeanVariance:") parseArray(iss, params.m_rollAngleMeanVariance, 2);
		else if (key == "m_apicalAngleMeanVariance:") parseArray(iss, params.m_apicalAngleMeanVariance, 2);
		else if (key == "m_gravitropism:") iss >> params.m_gravitropism;
		else if (key == "m_phototropism:") iss >> params.m_phototropism;
		else if (key == "m_internodeLength:") iss >> params.m_internodeLength;
		else if (key == "m_endNodeThickness:") iss >> params.m_endNodeThickness;
		else if (key == "m_thicknessAccumulationFactor:") iss >> params.m_thicknessAccumulationFactor;
		else if (key == "m_thicknessAccumulateAgeFactor:") iss >> params.m_thicknessAccumulateAgeFactor;
		else if (key == "m_lateralBudFlushingProbabilityTemperatureRange:") parseArray(iss, params.m_lateralBudFlushingProbabilityTemperatureRange, 4);
		else if (key == "m_leafBudFlushingProbabilityTemperatureRange:") parseArray(iss, params.m_leafBudFlushingProbabilityTemperatureRange, 4);
		else if (key == "m_fruitBudFlushingProbabilityTemperatureRange:") parseArray(iss, params.m_fruitBudFlushingProbabilityTemperatureRange, 4);
		else if (key == "m_apicalBudLightingFactor:") iss >> params.m_apicalBudLightingFactor;
		else if (key == "m_lateralBudLightingFactor:") iss >> params.m_lateralBudLightingFactor;
		else if (key == "m_leafBudLightingFactor:") iss >> params.m_leafBudLightingFactor;
		else if (key == "m_fruitBudLightingFactor:") iss >> params.m_fruitBudLightingFactor;
		else if (key == "m_apicalControl:") iss >> params.m_apicalControl;
		else if (key == "m_apicalControlAgeFactor:") iss >> params.m_apicalControlAgeFactor;
		else if (key == "m_apicalDominance:") iss >> params.m_apicalDominance;
		else if (key == "m_apicalDominanceAgeFactor:") iss >> params.m_apicalDominanceAgeFactor;
		else if (key == "m_apicalDominanceDistanceFactor:") iss >> params.m_apicalDominanceDistanceFactor;
		else if (key == "m_apicalBudExtinctionRate:") iss >> params.m_apicalBudExtinctionRate;
		else if (key == "m_lateralBudExtinctionRate:") iss >> params.m_lateralBudExtinctionRate;
		else if (key == "m_leafBudExtinctionRate:") iss >> params.m_leafBudExtinctionRate;
		else if (key == "m_fruitBudExtinctionRate:") iss >> params.m_fruitBudExtinctionRate;
		else if (key == "m_leafVigorRequirement:") iss >> params.m_leafVigorRequirement;
		else if (key == "m_fruitVigorRequirement:") iss >> params.m_fruitVigorRequirement;
		else if (key == "m_internodeVigorRequirement:") iss >> params.m_internodeVigorRequirement;
		else if (key == "m_vigorRequirementAggregateLoss:") iss >> params.m_vigorRequirementAggregateLoss;
		else if (key == "m_lowBranchPruning:") iss >> params.m_lowBranchPruning;
		else if (key == "m_saggingFactorThicknessReductionMax:") parseArray(iss, params.m_saggingFactorThicknessReductionMax, 3);
		else if (key == "m_maxLeafSize:") parseArray(iss, params.m_maxLeafSize, 3);
		else if (key == "m_leafPositionVariance:") iss >> params.m_leafPositionVariance;
		else if (key == "m_leafRandomRotation:") iss >> params.m_leafRandomRotation;
		else if (key == "m_leafChlorophyllLoss:") iss >> params.m_leafChlorophyllLoss;
		else if (key == "m_leafChlorophyllSynthesisFactorTemperature:") iss >> params.m_leafChlorophyllSynthesisFactorTemperature;
		else if (key == "m_leafFallProbability:") iss >> params.m_leafFallProbability;
		else if (key == "m_leafDistanceToBranchEndLimit:") iss >> params.m_leafDistanceToBranchEndLimit;
		else if (key == "m_maxFruitSize:") parseArray(iss, params.m_maxFruitSize, 3);
		else if (key == "m_fruitPositionVariance:") iss >> params.m_fruitPositionVariance;
		else if (key == "m_fruitRandomRotation:") iss >> params.m_fruitRandomRotation;
		else if (key == "m_fruitFallProbability:") iss >> params.m_fruitFallProbability;
		else if (key == "}") break; // End of shoot growth parameters
	}
}

void parseRootGrowthParameters(std::ifstream& file, RGParams& params) {
	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string key;
		iss >> key;

		if (key == "m_branchingAngleMeanVariance:") parseArray(iss, params.m_branchingAngleMeanVariance, 2);
		else if (key == "m_rollAngleMeanVariance:") parseArray(iss, params.m_rollAngleMeanVariance, 2);
		else if (key == "m_apicalAngleMeanVariance:") parseArray(iss, params.m_apicalAngleMeanVariance, 2);
		else if (key == "m_rootNodeLength:") iss >> params.m_rootNodeLength;
		else if (key == "m_rootNodeGrowthRate:") iss >> params.m_rootNodeGrowthRate;
		else if (key == "m_endNodeThickness:") iss >> params.m_endNodeThickness;
		else if (key == "m_thicknessAccumulationFactor:") iss >> params.m_thicknessAccumulationFactor;
		else if (key == "m_thicknessAccumulateAgeFactor:") iss >> params.m_thicknessAccumulateAgeFactor;
		else if (key == "m_rootNodeVigorRequirement:") iss >> params.m_rootNodeVigorRequirement;
		else if (key == "m_vigorRequirementAggregateLoss:") iss >> params.m_vigorRequirementAggregateLoss;
		else if (key == "m_environmentalFriction:") iss >> params.m_environmentalFriction;
		else if (key == "m_environmentalFrictionFactor:") iss >> params.m_environmentalFrictionFactor;
		else if (key == "m_apicalControl:") iss >> params.m_apicalControl;
		else if (key == "m_apicalControlAgeFactor:") iss >> params.m_apicalControlAgeFactor;
		else if (key == "m_apicalDominance:") iss >> params.m_apicalDominance;
		else if (key == "m_apicalDominanceAgeFactor:") iss >> params.m_apicalDominanceAgeFactor;
		else if (key == "m_apicalDominanceDistanceFactor:") iss >> params.m_apicalDominanceDistanceFactor;
		else if (key == "m_tropismSwitchingProbability:") iss >> params.m_tropismSwitchingProbability;
		else if (key == "m_tropismSwitchingProbabilityDistanceFactor:") iss >> params.m_tropismSwitchingProbabilityDistanceFactor;
		else if (key == "m_tropismIntensity:") iss >> params.m_tropismIntensity;
		else if (key == "m_branchingProbability:") iss >> params.m_branchingProbability;
		else if (key == "}") break; // End of root growth parameters
	}
}

bool parseConfigFile(const std::string& filename, PlantParameters& params) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error: Could not open file " << filename << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string key;
		iss >> key;

		if (key == "m_shootGrowthParameters:") {
			parseShootGrowthParameters(file, params.sg);
		}
		else if (key == "m_rootGrowthParameters:") {
			parseRootGrowthParameters(file, params.rg);
		}
	}
	file.close();
	return true;
}

void TreeNode::SetSoilLayer(SoilLayer& sl) {

	sl.m_mat = SoilPhysicalMaterial({ 1,
		[](const glm::vec3& pos) { return 1.0f; },
		[](const glm::vec3& pos) { return 0.0f; },
		[](const glm::vec3& pos) { return 0.0f; },
		[](const glm::vec3& pos) { return 0.0f; },
		[](const glm::vec3& pos) { return 0.0f; } });
	sl.m_mat.m_soilMaterialTexture = std::make_shared<SoilMaterialTexture>();
	sl.m_mat.m_soilMaterialTexture->m_color_map.resize(2 * 2);
	std::fill(sl.m_mat.m_soilMaterialTexture->m_color_map.begin(), sl.m_mat.m_soilMaterialTexture->m_color_map.end(), glm::vec4(0, 0, 0, 1));
	sl.m_mat.m_soilMaterialTexture->m_height_map.resize(4);
	std::fill(sl.m_mat.m_soilMaterialTexture->m_height_map.begin(), sl.m_mat.m_soilMaterialTexture->m_height_map.end(), 1.0f);
	sl.m_mat.m_soilMaterialTexture->m_metallic_map.resize(4);
	std::fill(sl.m_mat.m_soilMaterialTexture->m_metallic_map.begin(), sl.m_mat.m_soilMaterialTexture->m_metallic_map.end(), 0.2f);
	sl.m_mat.m_soilMaterialTexture->m_roughness_map.resize(4);
	std::fill(sl.m_mat.m_soilMaterialTexture->m_roughness_map.begin(), sl.m_mat.m_soilMaterialTexture->m_roughness_map.end(), 0.8f);
	sl.m_mat.m_soilMaterialTexture->m_normal_map.resize(4);
	std::fill(sl.m_mat.m_soilMaterialTexture->m_normal_map.begin(), sl.m_mat.m_soilMaterialTexture->m_normal_map.end(), glm::vec3(0, 0, 1));
	sl.m_thickness = [](const glm::vec2& position) {return 1000.f; };
}

bool TreeNode::ReadTreeFile(const std::string& fileName, RootGrowthController& m_rootGrowthParameters, ShootGrowthController& m_shootGrowthParameters,
	ClimateModel& cm) {
	PlantParameters params;
	if (!parseConfigFile(fileName, params)) {
		return false;
	}
	MGlobal::displayInfo("Reading Tree File");
	// ROOT - elm
	m_rootGrowthParameters.m_apicalAngle = [=](const Node<RootNodeGrowthData>& rootNode)
		{
			// This is from apicalAngleMeanVariance
			return glm::gaussRand(params.rg.m_apicalAngleMeanVariance[0], params.rg.m_apicalAngleMeanVariance[1]);
		};
	// 1 + apicalControl * glm::exp(-apicalControlAgeFactor, m_age)
	m_rootGrowthParameters.m_apicalControl = 1 + params.rg.m_apicalControl * glm::exp(-params.rg.m_apicalControlAgeFactor * treeModel.m_age);
	m_rootGrowthParameters.m_apicalDominance = [=](const Node<RootNodeGrowthData>& rootNode)
		{
			// apicalDominance * glm::exp(-apicalDominanceAgeFactor * m_age)
			return params.rg.m_apicalDominance * glm::exp(-params.rg.m_apicalDominanceAgeFactor * treeModel.m_age);	//apical dominance age factor is 0 for elm -> whole thing is 0
		};
	m_rootGrowthParameters.m_apicalDominanceDistanceFactor = params.rg.m_apicalDominanceDistanceFactor;
	m_rootGrowthParameters.m_branchingAngle = [=](const Node<RootNodeGrowthData>& rootNode)
		{
			// breanchingAngleMeanVariance
			return glm::gaussRand(params.rg.m_branchingAngleMeanVariance[0], params.rg.m_branchingAngleMeanVariance[1]);
		};
	m_rootGrowthParameters.m_branchingProbability = [=](const Node<RootNodeGrowthData>& rootNode)
		{
			return  params.rg.m_branchingProbability;
		};
	m_rootGrowthParameters.m_endNodeThickness = params.rg.m_endNodeThickness;
	m_rootGrowthParameters.m_environmentalFriction = [=](const Node<RootNodeGrowthData>& rootNode)
		{
			const auto& rootNodeData = rootNode.m_data;
			// glm::pow(1.0 / glm::max(rootNodeData.m_soilDensity * m_environmentalFritcion, 1.0), m_environmentalFrictionFactor)
			return 1.0f - glm::pow(1.0f / glm::max(rootNodeData.m_soilDensity * params.rg.m_environmentalFriction, 1.f), params.rg.m_environmentalFrictionFactor);
		};

	// TODO: WTF ARE THESE
	// edit them because they are used, but idk where they come from
	m_rootGrowthParameters.m_fineRootApicalAngleVariance = 0.3;
	m_rootGrowthParameters.m_fineRootBranchingAngle = 30;
	m_rootGrowthParameters.m_fineRootMinNodeThickness = 0.1;
	m_rootGrowthParameters.m_fineRootNodeCount = 5;
	m_rootGrowthParameters.m_fineRootSegmentLength = 1.2;
	m_rootGrowthParameters.m_fineRootThickness = 0.5;

	m_rootGrowthParameters.m_rollAngle = [=](const Node<RootNodeGrowthData>& rootNode)
		{
			//m_rollAngleMeanVariance
			return glm::gaussRand(params.rg.m_rollAngleMeanVariance[0], params.rg.m_rollAngleMeanVariance[1]);
		};
	m_rootGrowthParameters.m_rootNodeGrowthRate = params.rg.m_rootNodeGrowthRate;
	m_rootGrowthParameters.m_rootNodeLength = params.rg.m_rootNodeLength;
	m_rootGrowthParameters.m_rootNodeVigorRequirement = params.rg.m_rootNodeVigorRequirement;
	m_rootGrowthParameters.m_thicknessAccumulateAgeFactor = params.rg.m_thicknessAccumulateAgeFactor;
	m_rootGrowthParameters.m_thicknessAccumulationFactor = params.rg.m_thicknessAccumulationFactor;
	m_rootGrowthParameters.m_tropismIntensity = params.rg.m_tropismIntensity;
	m_rootGrowthParameters.m_tropismSwitchingProbability = params.rg.m_tropismSwitchingProbability;
	m_rootGrowthParameters.m_tropismSwitchingProbabilityDistanceFactor = params.rg.m_tropismSwitchingProbabilityDistanceFactor;
	m_rootGrowthParameters.m_vigorRequirementAggregateLoss = params.rg.m_vigorRequirementAggregateLoss;

	// SHOOT
	m_shootGrowthParameters.m_internodeGrowthRate = params.sg.m_internodeGrowthRate;
	m_shootGrowthParameters.m_leafGrowthRate = params.sg.m_leafGrowthRate;
	m_shootGrowthParameters.m_fruitGrowthRate = params.sg.m_fruitGrowthRate;
	m_shootGrowthParameters.m_lateralBudCount = params.sg.m_lateralBudCount;
	m_shootGrowthParameters.m_fruitBudCount = params.sg.m_fruitBudCount;
	m_shootGrowthParameters.m_leafBudCount = params.sg.m_leafBudCount;
	m_shootGrowthParameters.m_branchingAngle = [=](const Node<InternodeGrowthData>& internode)
		{
			return glm::gaussRand(params.sg.m_branchingAngleMeanVariance[0], params.sg.m_branchingAngleMeanVariance[1]);
		};
	m_shootGrowthParameters.m_rollAngle = [=](const Node<InternodeGrowthData>& internode)
		{
			return glm::gaussRand(params.sg.m_rollAngleMeanVariance[0], params.sg.m_rollAngleMeanVariance[1]);
		};
	m_shootGrowthParameters.m_apicalAngle = [=](const Node<InternodeGrowthData>& internode)
		{
			return glm::gaussRand(params.sg.m_apicalAngleMeanVariance[0], params.sg.m_apicalAngleMeanVariance[1]);
		};
	m_shootGrowthParameters.m_gravitropism = [=](const Node<InternodeGrowthData>& internode)
		{
			return params.sg.m_gravitropism;
		};
	m_shootGrowthParameters.m_phototropism = [=](const Node<InternodeGrowthData>& internode)
		{
			return params.sg.m_phototropism;
		};
	m_shootGrowthParameters.m_internodeLength = params.sg.m_internodeLength;
	m_shootGrowthParameters.m_endNodeThickness = params.sg.m_endNodeThickness;
	m_shootGrowthParameters.m_thicknessAccumulationFactor = params.sg.m_thicknessAccumulationFactor;
	m_shootGrowthParameters.m_thicknessAccumulateAgeFactor = params.sg.m_thicknessAccumulateAgeFactor;

	m_shootGrowthParameters.m_lateralBudFlushingProbability = [=](const Node<InternodeGrowthData>& internode)
		{
			glm::vec4 probabilityRange = glm::vec4(params.sg.m_lateralBudFlushingProbabilityTemperatureRange[0],
				params.sg.m_lateralBudFlushingProbabilityTemperatureRange[1],
				params.sg.m_lateralBudFlushingProbabilityTemperatureRange[2],
				params.sg.m_lateralBudFlushingProbabilityTemperatureRange[3]);
			const auto& internodeData = internode.m_data;
			float flushProbability = glm::mix(probabilityRange.x, probabilityRange.y,
				glm::clamp((internodeData.m_temperature - probabilityRange.z) / (probabilityRange.w - probabilityRange.z), 0.0f, 1.0f));
			if (internodeData.m_inhibitor > 0.0f) flushProbability *= glm::exp(-internodeData.m_inhibitor);
			//1 is shootGrowthParameters.m_lateralBudLightingFactor:
			flushProbability *= glm::pow(internodeData.m_lightIntensity, params.sg.m_lateralBudLightingFactor);
			return flushProbability;
		};
	m_shootGrowthParameters.m_leafBudFlushingProbability = [=](const Node<InternodeGrowthData>& internode)
		{
			const auto& internodeData = internode.m_data;
			const auto& probabilityRange = glm::vec4(params.sg.m_leafBudFlushingProbabilityTemperatureRange[0],
				params.sg.m_leafBudFlushingProbabilityTemperatureRange[1],
				params.sg.m_leafBudFlushingProbabilityTemperatureRange[2],
				params.sg.m_leafBudFlushingProbabilityTemperatureRange[3]);
			float flushProbability = glm::mix(probabilityRange.x, probabilityRange.y,
				glm::clamp((internodeData.m_temperature - probabilityRange.z) / (probabilityRange.w - probabilityRange.z), 0.0f, 1.0f));
			//1 is shootGrowthParameters.m_leafBudLightingFactor:
			flushProbability *= glm::pow(internodeData.m_lightIntensity, params.sg.m_leafBudLightingFactor);
			return flushProbability;
		};
	m_shootGrowthParameters.m_fruitBudFlushingProbability = [=](const Node<InternodeGrowthData>& internode)
		{
			const auto& internodeData = internode.m_data;
			const auto& probabilityRange = glm::vec4(params.sg.m_fruitBudFlushingProbabilityTemperatureRange[0],
				params.sg.m_fruitBudFlushingProbabilityTemperatureRange[1],
				params.sg.m_fruitBudFlushingProbabilityTemperatureRange[2],
				params.sg.m_fruitBudFlushingProbabilityTemperatureRange[3]);
			float flushProbability = glm::mix(probabilityRange.x, probabilityRange.y,
				glm::clamp((internodeData.m_temperature - probabilityRange.z) / (probabilityRange.w - probabilityRange.z), 0.0f, 1.0f));
			//1 is shootGrowthParameters.m_fruitBudLightingFactor:
			flushProbability *= glm::pow(internodeData.m_lightIntensity, params.sg.m_fruitBudLightingFactor);
			return flushProbability;
		};

	//m_shootGrowthParameters.m_apicalBudLightingFactor = 0															;
	//m_shootGrowthParameters.m_lateralBudLightingFactor = 1															;
	//m_shootGrowthParameters.m_leafBudLightingFactor = 1																;
	//m_shootGrowthParameters.m_fruitBudLightingFactor = 1															;
	// m_shootGrowthParameters.m_apicalControl = 0.75;

	m_shootGrowthParameters.m_apicalControl =
		1.0f + params.sg.m_apicalControl * glm::exp(-params.sg.m_apicalControlAgeFactor * treeModel.m_age);

	m_shootGrowthParameters.m_apicalDominance = [=](const Node<InternodeGrowthData>& internode)
		{
			return params.sg.m_apicalDominance * glm::exp(params.sg.m_apicalDominanceAgeFactor * treeModel.m_age);
		};
	m_shootGrowthParameters.m_apicalDominanceDistanceFactor = params.sg.m_apicalDominanceDistanceFactor;
	m_shootGrowthParameters.m_leafVigorRequirement = params.sg.m_leafVigorRequirement;
	m_shootGrowthParameters.m_fruitVigorRequirement = params.sg.m_fruitVigorRequirement;
	m_shootGrowthParameters.m_internodeVigorRequirement = params.sg.m_internodeVigorRequirement;
	m_shootGrowthParameters.m_vigorRequirementAggregateLoss = params.sg.m_vigorRequirementAggregateLoss;
	m_shootGrowthParameters.m_lowBranchPruning = params.sg.m_lowBranchPruning;

	m_shootGrowthParameters.m_sagging = [=](const Node<InternodeGrowthData>& internode)
		{
			glm::vec3 m_saggingFactorThicknessReductionMax = glm::vec3(params.sg.m_saggingFactorThicknessReductionMax[0],
				params.sg.m_saggingFactorThicknessReductionMax[1],
				params.sg.m_saggingFactorThicknessReductionMax[2]);
			const float newSagging = glm::min(
				m_saggingFactorThicknessReductionMax.z,
				m_saggingFactorThicknessReductionMax.x *
				(internode.m_data.m_descendentTotalBiomass + internode.m_data.m_extraMass) /
				glm::pow(
					internode.m_info.m_thickness /
					//endnodethickness:
					params.sg.m_endNodeThickness,
					m_saggingFactorThicknessReductionMax.y));
			return glm::max(internode.m_data.m_sagging, newSagging);
		};

	m_shootGrowthParameters.m_maxLeafSize = { params.sg.m_maxLeafSize[0],\
		params.sg.m_maxLeafSize[1],
		params.sg.m_maxLeafSize[2]};
	m_shootGrowthParameters.m_leafPositionVariance = params.sg.m_leafPositionVariance;
	//m_shootGrowthParameters.m_leafRandomRotation = 10																;
	m_shootGrowthParameters.m_leafDamage = [=](const Node<InternodeGrowthData>& internode)
		{
			const auto& internodeData = internode.m_data;
			float leafDamage = 0.0f;
			//shootGrowthParameters.m_leafChlorophyllSynthesisFactorTemperature is 65
			if (cm.m_time - glm::floor(cm.m_time) > 0.5f && internodeData.m_temperature < 65)
			{
				//4 is m_leafChlorophyllLoss
				leafDamage += params.sg.m_leafChlorophyllLoss;
			}
			return leafDamage;
		};
	m_shootGrowthParameters.m_leafFallProbability = [=](const Node<InternodeGrowthData>& internode)
		{
			return params.sg.m_leafFallProbability;
		};
	m_shootGrowthParameters.m_maxFruitSize = { params.sg.m_maxFruitSize[0],
		params.sg.m_maxFruitSize[1],
		params.sg.m_maxFruitSize[2]};
	m_shootGrowthParameters.m_fruitPositionVariance = params.sg.m_fruitPositionVariance;
	//m_shootGrowthParameters.m_fruitRandomRotation = 10																;
	m_shootGrowthParameters.m_fruitFallProbability = [=](const Node<InternodeGrowthData>& internode)
		{
			return params.sg.m_fruitFallProbability;
		};
	m_shootGrowthParameters.m_pruningFactor = [=](const Node<InternodeGrowthData>& internode)
		{
			float pruningProbability = 0.0f;
			const auto& internodeData = internode.m_data;
			if (internode.IsEndNode())
			{
				pruningProbability = (1.0f - internode.m_data.m_lightIntensity) * 0.1;
			}
			return pruningProbability;
		};
	return true;
}

void TreeNode::InitializeMVars(const std::string& treeFilePath, SoilModel& m_soilModel, ClimateModel& m_climateModel,
	RootGrowthController& m_rootGrowthParameters, ShootGrowthController& m_shootGrowthParameters) {
	// SOIL
	SoilParameters sp = SoilParameters();
	SoilSurface sf = SoilSurface();
	sf.m_height = [&](const glm::vec2& position)
		{
			return 0.0f;
		};
	SoilLayer sl;
	sl.m_thickness = [&](const glm::vec2& pos)
		{
			return 1.0f;
		};
	SetSoilLayer(sl);
	std::vector<SoilLayer> sls = { sl };
	m_soilModel.Initialize(sp, sf, sls);

	// CLIMATE
	ClimateParameters climateParametrs;
	m_climateModel.Initialize(climateParametrs);

	// if we don't define a file path or couldn't read the file, we will initialize it as an elm
	if (treeFilePath == "" || !ReadTreeFile(treeFilePath, m_rootGrowthParameters, m_shootGrowthParameters, m_climateModel)) {
		// ROOT - elm
		m_rootGrowthParameters.m_apicalAngle = [=](const Node<RootNodeGrowthData>& rootNode)
			{
				// This is from apicalAngleMeanVariance
				return glm::gaussRand(0, 3);
			};
		// 1 + apicalControl * glm::exp(-apicalControlAgeFactor * m_age)
		m_rootGrowthParameters.m_apicalControl = 1 + 0;
		m_rootGrowthParameters.m_apicalDominance = [=](const Node<RootNodeGrowthData>& rootNode)
			{
				// apicalDominance * glm::exp(-apicalDominanceAgeFactor, m_age)
				return 0;	//apical dominance age factor is 0 for elm -> whole thing is 0
			};
		m_rootGrowthParameters.m_apicalDominanceDistanceFactor = 0.970000029;
		m_rootGrowthParameters.m_branchingAngle = [=](const Node<RootNodeGrowthData>& rootNode)
			{
				// breanchingAngleMeanVariance
				return glm::gaussRand(30, 2);
			};
		m_rootGrowthParameters.m_branchingProbability = [=](const Node<RootNodeGrowthData>& rootNode)
			{
				return  0.0700000003;
			};
		m_rootGrowthParameters.m_endNodeThickness = 0.00200000009;
		m_rootGrowthParameters.m_environmentalFriction = [=](const Node<RootNodeGrowthData>& rootNode)
			{
				const auto& rootNodeData = rootNode.m_data;
				// glm::pow(1.0 / glm::max(rootNodeData.m_soilDensity * m_environmentalFritcion, 1.0), m_environmentalFrictionFactor)
				return 1.0f - glm::pow(1.0f / glm::max(rootNodeData.m_soilDensity * 0.5, 1.0), 1.0);
			};
		m_rootGrowthParameters.m_fineRootApicalAngleVariance = 0.3;
		m_rootGrowthParameters.m_fineRootBranchingAngle = 30;
		m_rootGrowthParameters.m_fineRootMinNodeThickness = 0.1;
		m_rootGrowthParameters.m_fineRootNodeCount = 5;
		m_rootGrowthParameters.m_fineRootSegmentLength = 1.2;
		m_rootGrowthParameters.m_fineRootThickness = 0.5;
		m_rootGrowthParameters.m_rollAngle = [=](const Node<RootNodeGrowthData>& rootNode)
			{
				//m_rollAngleMeanVariance
				return glm::gaussRand(120, 2);
			};
		m_rootGrowthParameters.m_rootNodeGrowthRate = 10;
		m_rootGrowthParameters.m_rootNodeLength = 0.0299999993;
		m_rootGrowthParameters.m_rootNodeVigorRequirement = 1;
		m_rootGrowthParameters.m_thicknessAccumulateAgeFactor = 0;
		m_rootGrowthParameters.m_thicknessAccumulationFactor = 0.5;
		m_rootGrowthParameters.m_tropismIntensity = 0.0500000007;
		m_rootGrowthParameters.m_tropismSwitchingProbability = 1;
		m_rootGrowthParameters.m_tropismSwitchingProbabilityDistanceFactor = 0;
		m_rootGrowthParameters.m_vigorRequirementAggregateLoss = 1;

		// SHOOT - Elm
		m_shootGrowthParameters.m_internodeGrowthRate = 10;
		m_shootGrowthParameters.m_leafGrowthRate = 18;
		m_shootGrowthParameters.m_fruitGrowthRate = 18;
		m_shootGrowthParameters.m_lateralBudCount = 2;
		m_shootGrowthParameters.m_fruitBudCount = 0;
		m_shootGrowthParameters.m_leafBudCount = 1;
		m_shootGrowthParameters.m_branchingAngle = [=](const Node<InternodeGrowthData>& internode)
			{
				return glm::gaussRand(60, 3);
			};
		m_shootGrowthParameters.m_rollAngle = [=](const Node<InternodeGrowthData>& internode)
			{
				return glm::gaussRand(90, 2);
			};
		m_shootGrowthParameters.m_apicalAngle = [=](const Node<InternodeGrowthData>& internode)
			{
				return glm::gaussRand(0, 2);
			};
		m_shootGrowthParameters.m_gravitropism = [=](const Node<InternodeGrowthData>& internode)
			{
				return 0.0299999993;
			};
		m_shootGrowthParameters.m_phototropism = [=](const Node<InternodeGrowthData>& internode)
			{
				return 0.0500000007;
			};
		m_shootGrowthParameters.m_internodeLength = 0.0299999993;
		m_shootGrowthParameters.m_endNodeThickness = 0.00200000009;
		m_shootGrowthParameters.m_thicknessAccumulationFactor = 0.5;
		m_shootGrowthParameters.m_thicknessAccumulateAgeFactor = 0;

		m_shootGrowthParameters.m_lateralBudFlushingProbability = [=](const Node<InternodeGrowthData>& internode)
			{
				glm::vec4 probabilityRange = glm::vec4(0.00499999989, 0.00499999989, 0, 100);
				const auto& internodeData = internode.m_data;
				float flushProbability = glm::mix(probabilityRange.x, probabilityRange.y,
					glm::clamp((internodeData.m_temperature - probabilityRange.z) / (probabilityRange.w - probabilityRange.z), 0.0f, 1.0f));
				if (internodeData.m_inhibitor > 0.0f) flushProbability *= glm::exp(-internodeData.m_inhibitor);
				//1 is shootGrowthParameters.m_lateralBudLightingFactor:
				flushProbability *= glm::pow(internodeData.m_lightIntensity, 1);
				return flushProbability;
			};
		m_shootGrowthParameters.m_leafBudFlushingProbability = [=](const Node<InternodeGrowthData>& internode)
			{
				const auto& internodeData = internode.m_data;
				const auto& probabilityRange = glm::vec4(1, 1, 45, 60);
				float flushProbability = glm::mix(probabilityRange.x, probabilityRange.y,
					glm::clamp((internodeData.m_temperature - probabilityRange.z) / (probabilityRange.w - probabilityRange.z), 0.0f, 1.0f));
				//1 is shootGrowthParameters.m_leafBudLightingFactor:
				flushProbability *= glm::pow(internodeData.m_lightIntensity, 1);
				return flushProbability;
			};
		m_shootGrowthParameters.m_fruitBudFlushingProbability = [=](const Node<InternodeGrowthData>& internode)
			{
				const auto& internodeData = internode.m_data;
				const auto& probabilityRange = glm::vec4(0, 1, 50, 70);
				float flushProbability = glm::mix(probabilityRange.x, probabilityRange.y,
					glm::clamp((internodeData.m_temperature - probabilityRange.z) / (probabilityRange.w - probabilityRange.z), 0.0f, 1.0f));
				//1 is shootGrowthParameters.m_fruitBudLightingFactor:
				flushProbability *= glm::pow(internodeData.m_lightIntensity, 1);
				return flushProbability;
			};

		//m_shootGrowthParameters.m_apicalBudLightingFactor = 0															;
		//m_shootGrowthParameters.m_lateralBudLightingFactor = 1															;
		//m_shootGrowthParameters.m_leafBudLightingFactor = 1																;
		//m_shootGrowthParameters.m_fruitBudLightingFactor = 1															;
		// m_shootGrowthParameters.m_apicalControl = 0.75;

		m_shootGrowthParameters.m_apicalControl =
			1.0f + 0.75 * glm::exp(-0.159999996 * treeModel.m_age);

		m_shootGrowthParameters.m_apicalDominance = [=](const Node<InternodeGrowthData>& internode)
			{
				return 0.349999994 * glm::exp(0 * treeModel.m_age);
			};
		m_shootGrowthParameters.m_apicalDominanceDistanceFactor = 0.999000013;
		m_shootGrowthParameters.m_leafVigorRequirement = 0.100000001;
		m_shootGrowthParameters.m_fruitVigorRequirement = 0.100000001;
		m_shootGrowthParameters.m_internodeVigorRequirement = 1;
		m_shootGrowthParameters.m_vigorRequirementAggregateLoss = 0.949999988;
		m_shootGrowthParameters.m_lowBranchPruning = 0.0500000007;

		m_shootGrowthParameters.m_sagging = [=](const Node<InternodeGrowthData>& internode)
			{
				const auto& m_saggingFactorThicknessReductionMax = glm::vec3(0.00499999989, 3, 0.300000012);
				const float newSagging = glm::min(
					0.300000012,
					m_saggingFactorThicknessReductionMax.x *
					(internode.m_data.m_descendentTotalBiomass + internode.m_data.m_extraMass) /
					glm::pow(
						internode.m_info.m_thickness /
						//endnodethickness:
						0.00200000009,
						m_saggingFactorThicknessReductionMax.y));
				return glm::max(internode.m_data.m_sagging, newSagging);
			};

		m_shootGrowthParameters.m_maxLeafSize = { 0.0500000007, 0.5, 0.0500000007 };
		m_shootGrowthParameters.m_leafPositionVariance = 0.5;
		//m_shootGrowthParameters.m_leafRandomRotation = 10																;
		m_shootGrowthParameters.m_leafDamage = [=](const Node<InternodeGrowthData>& internode)
			{
				const auto& internodeData = internode.m_data;
				float leafDamage = 0.0f;
				//shootGrowthParameters.m_leafChlorophyllSynthesisFactorTemperature is 65
				if (m_climateModel.m_time - glm::floor(m_climateModel.m_time) > 0.5f && internodeData.m_temperature < 65)
				{
					//4 is m_leafChlorophyllLoss
					leafDamage += 4;
				}
				return leafDamage;
			};
		m_shootGrowthParameters.m_leafFallProbability = [=](const Node<InternodeGrowthData>& internode)
			{
				return 3;
			};
		m_shootGrowthParameters.m_maxFruitSize = { 0.0350000001, 0.0350000001, 0.0350000001 };
		m_shootGrowthParameters.m_fruitPositionVariance = 0.5;
		//m_shootGrowthParameters.m_fruitRandomRotation = 10																;
		m_shootGrowthParameters.m_fruitFallProbability = [=](const Node<InternodeGrowthData>& internode)
			{
				return 3;
			};
		m_shootGrowthParameters.m_pruningFactor = [=](const Node<InternodeGrowthData>& internode)
			{
				return 0.0;
			};
	}

	MGlobal::displayInfo("Apical Dom Dist Factor:");
	MGlobal::displayInfo(std::to_string(m_shootGrowthParameters.m_apicalDominanceDistanceFactor).c_str());

	MGlobal::displayInfo("End Node Thickness:");
	MGlobal::displayInfo(std::to_string(m_shootGrowthParameters.m_endNodeThickness).c_str());

	MGlobal::displayInfo("Tree Initialized");
}