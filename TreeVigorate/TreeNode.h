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

#define mPrint(msg) MGlobal::displayInfo(msg)

using namespace EcoSysLab;

#define McheckErr(stat,msg)         \
    if ( MS::kSuccess != stat ) {   \
        cerr << msg;                \
        return MS::kFailure;        \
    }

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
	/// <param name="treeFilePath"> full directory path to a .td tree species file to read in</param>
	/// <param name="m_soilModel"></param>
	/// <param name="m_climateModel"></param>
	/// <param name="m_rootGrowthParameters"></param>
	/// <param name="m_shootGrowthParameters"></param>
	void InitializeMVars(const std::string& treeFilePath, SoilModel &m_soilModel, ClimateModel& m_climateModel,
		RootGrowthController& m_rootGrowthParameters, ShootGrowthController& m_shootGrowthParameters);
	
	/// <summary>
	/// The tree model that we're updating. This is a member var and I hope this works well like this.
	/// </summary>
	TreeModel treeModel;
	
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
	bool appendNodeCylindersToMesh( MPointArray& vertices, MIntArray& tris, MIntArray& edges, ShootSkeleton& skeleton, double rad);
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
	bool ReadTreeFile(const std::string& fileName, RootGrowthController& m_rootGrowthParameters, ShootGrowthController& m_shootGrowthParameters, ClimateModel& cm);

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
};