#pragma once
///	
/// Created by Avi and Eli, Spring 2025
/// 

#include <maya/MStringArray.h>
#include <maya/MTime.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MPointArray.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnStringData.h>
#include <maya/MIOStream.h>
#include <cylinder.h>

#include "Rhizocode/TreeModel.hpp"
#include <StrandManager.h>
#include <maya/MGlobal.h>
#include <list>
#include <glm/gtc/random.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_set>

#define mPrint(msg) MGlobal::displayInfo(msg)

using namespace EcoSysLab;

#define McheckErr(stat,msg)         \
    if ( MS::kSuccess != stat ) {   \
        cerr << msg;                \
        return MS::kFailure;        \
    }

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

struct Controllers {
	SoilModel sm;
	ClimateModel cm;
	ShootGrowthController sgc;
	RootGrowthController rgc;
	bool isInit = false;
};

class TreeNode : public MPxNode {
private:
	/// <summary>
	/// Helper function that initializes a soil layer to basic values
	/// </summary>
	/// <param name="sl"> the soil layer to initialize </param>
	void SetSoilLayer(SoilLayer& sl);

	/// <summary>
	/// Function to initialize tree model with the set parameters
	/// </summary>
	/// <param name="tree"> full directory path to a .td tree species file to read in, or the string of a preset tree</param>
	/// <param name="m_soilModel"></param>
	/// <param name="m_climateModel"></param>
	/// <param name="m_rootGrowthParameters"></param>
	/// <param name="m_shootGrowthParameters"></param>
	void InitializeMVars(const std::string& tree, SoilModel &m_soilModel, ClimateModel& m_climateModel,
		RootGrowthController& m_rootGrowthParameters, ShootGrowthController& m_shootGrowthParameters, MDataBlock& data);
	
	/// <summary>
	/// The tree model that we're updating. This is a member var and I hope this works well like this.
	/// </summary>
	TreeModel treeModel;

	/// <summary>
	/// Stores the initialized tree params
	/// </summary>
	Controllers treeParams;

	/// <summary>
	/// Due to Maya being Maya, this is the best way of checking if Radius was changed...
	/// </summary>
	float prevRad = 1.0;

	/// <summary>
	/// Due to Maya being Maya, this is to keep track of only segment changing
	/// </summary>
	int prevSeg = 1;

	/// <summary>
	/// Builds a cylinder mesh into the input arrays
	/// </summary>
	/// <param name="start">start loc</param>
	/// <param name="end">end loc</param>
	/// <param name="sRad">start rad</param>
	/// <param name="eRad">end rad</param>
	/// <param name="sDir">start normal</param>
	/// <param name="eDir">end normal</param>
	/// <param name="points"></param>
	/// <param name="faceCounts"></param>
	/// <param name="faceConns"></param>
	/// <param name="drawTop"></param>
	/// <param name="drawBot"></param>
	void buildCylinderMesh(MPoint& start, MPoint& end, float sRad, float eRad, glm::vec3 sDir, glm::vec3 eDir,
		MPointArray& points, MIntArray& faceCounts, MIntArray& faceConns, bool drawTop, bool drawBot);
	
	/// <summary>
	/// Creates a cylinder between each node in the input skeleton and appends them to the mesh
	/// as described in the input arrays
	/// </summary>
	/// <param name="vertices"> verts of the mesh</param>
	/// <param name="tris"> triangles of the mesh</param>
	/// <param name="edges"> edges of the mesh</param>
	/// <param name="skeleton"> skeleton storing the nodes</param>
	/// <param name="rad"> radius multiplier of cylinders</param>
	/// <returns>true</returns>
	bool appendNodeCylindersToMesh( MPointArray& vertices, MIntArray& tris, MIntArray& edges, ShootSkeleton& skeleton, StrandManager& strandManager, double rad, int segments);


	/// <summary>
	/// 
	/// </summary>
	/// <param name="skeleton"></param>
	/// <param name="strandManager"></param>
	/// <param name="rad"></param>
	void appendLeavesToMesh(ShootSkeleton& skeleton, StrandManager strandManager, double rad);

	/// <summary>
	/// Outputs an MString loading bar to be displayed in the console.
	/// Based on current iteration out of total
	/// </summary>
	/// <param name="curr"></param>
	/// <param name="tot"></param>
	/// <param name="lightDir"></param>
	/// <returns></returns>
	MString getLoadBar(int curr, int tot);

	void developSubtree(ShootSkeleton& skeleton, const NodeHandle& first, glm::vec3& lightDir);

	/// <summary>
	/// Prunes the entire subtree from the skeleton
	/// </summary>
	/// <param name="skeleton"></param>
	/// <param name="first"></param>
	void pruneSubtree(ShootSkeleton& skeleton, const NodeHandle& first);

	/// <summary>
	/// Performs growShoots on the subtree
	/// </summary>
	/// <param name="globalTransform"></param>
	/// <param name="first"></param>
	/// <param name="subNodes"></param>
	/// <param name="climateModel"></param>
	/// <param name="shootGrowthParameters"></param>
	/// <param name="newShootGrowthRequirement"></param>
	/// <returns></returns>
	bool growSubShoots(const glm::mat4& globalTransform, const NodeHandle& first, std::unordered_set<NodeHandle> subNodes,
		ClimateModel& climateModel, const ShootGrowthController& shootGrowthParameters, PlantGrowthRequirement& newShootGrowthRequirement);

	/// <summary>
	/// Allocates shoot vigor to the subtree
	/// </summary>
	/// <param name="first"></param>
	/// <param name="subNodes"></param>
	/// <param name="shootGrowthParameters"></param>
	void SubTreeAllocateShootVigor(const NodeHandle& first, std::unordered_set<NodeHandle> subNodes, const ShootGrowthController& shootGrowthParameters);

	/// <summary>
	/// The post process part of growth.
	/// Used when growing the subtree.
	/// </summary>
	/// <param name="treeStructureChanged"></param>
	/// <param name="m_shootSkeleton"></param>
	/// <param name="subNodes"></param>
	void growPostProcess(bool treeStructureChanged, ShootSkeleton& m_shootSkeleton, std::unordered_set<NodeHandle> subNodes);

	/// <summary>
	/// Sets new set of parameters for this tree's settings
	/// </summary>
	/// <param name="tp">tree parameters to change</param>
	/// <param name="internodeGrow">internode growth rate</param>
	/// <param name="meanAngVar">branching mean angle variance</param>
	/// <param name="apicAngVar">apical angle variance</param>
	/// <param name="g">gravitropism</param>
	/// <param name="p">phototropism</param>
	/// <param name="apicDom">apical dominance</param>
	void setNewParams(Controllers& tp, int internodeGrow, glm::vec2 meanAngVar, float apicAngVar, float g, float p, float apicDom);
public:
	TreeNode() {};
	~TreeNode() override {};
	/// <summary>
	/// Default maya function called to compute the output mesh
	/// </summary>
	/// <param name="plug"></param>
	/// <param name="data"></param>
	/// <returns></returns>
	MStatus compute(const MPlug& plug, MDataBlock& data) override;
	static  void* creator();
	static  MStatus initialize();

	/// <summary>
	/// Reads in the tree species file info into these input structs
	/// </summary>
	/// <param name="fileName"> the file path of the species file</param>
	/// <param name="m_rootGrowthParameters"></param>
	/// <param name="m_shootGrowthParameters"></param>
	/// <param name="cm"></param>
	/// <returns>true if reading was successful</returns>
	bool ReadTreeParams(const std::string& fileName, RootGrowthController& m_rootGrowthParameters, ShootGrowthController& m_shootGrowthParameters, ClimateModel& cm, bool fromFile, MDataBlock& data);

	static MObject	outputMesh;
	static MTypeId	id;

	// Time step for growth function
	static MObject deltaTime;
	// number of growth steps to compute before new mesh
	static MObject numGrows;
	// radius multiplier of cylinders
	static MObject radius;
	// full path to tree data file
	static MObject treeDataFile;
	// used to make growth update
	static MObject makeGrow;
	// used to set the sun direction
	static MObject sunDir;
	// used to keep track of how much the tree has grown
	static MObject growTime;
	// how many segments to do per flow when interpolating positions
	static MObject segments;

	// number of flow nodes
	static MObject numNodes;
	// selected node
	static MObject selectedNode;
	// how many times we want to grow the subnode
	static MObject numSubGrows;
	// the mesh for the node
	static MObject outputNodeMesh;
	// a bool to check if only node stuff should be updated
	static MObject growNode;
	// a bool to check if we want to prune the subtree
	static MObject pruneNode;

	// ADVANCED TREE PARAM STUFF
	static MObject internodeGrowth;
	static MObject meanAngleVar1;
	static MObject meanAngleVar2;
	static MObject apicalAngleVar;
	static MObject gravitrope;
	static MObject photo;
	static MObject apicDom;
	static MObject isParamChanged;
};