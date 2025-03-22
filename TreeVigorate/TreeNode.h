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
#include <maya/MGlobal.h>
#include <list>
#include <glm/gtc/random.hpp>

using namespace EcoSysLab;

#define McheckErr(stat,msg)         \
    if ( MS::kSuccess != stat ) {   \
        cerr << msg;                \
        return MS::kFailure;        \
    }

class TreeNode : public MPxNode {
private:
	void SetSoilLayer(SoilLayer& sl);
	void InitializeMVars(SoilModel &m_soilModel, ClimateModel& m_climateModel, 
		RootGrowthController& m_rootGrowthParameters, ShootGrowthController& m_shootGrowthParameters);
	TreeModel treeModel;
	bool addNodePositionPairs(Node<InternodeGrowthData>& currentNode, MPointArray& vertices, MIntArray& tris, MIntArray& edges, ShootSkeleton& skeleton);
	//static SoilModel m_soilModel;
	//static ClimateModel m_climateModel;
	//static RootGrowthController m_rootGrowthParameters;
	//static ShootGrowthController m_shootGrowthParameters;
public:
	TreeNode() {};
	~TreeNode() override {};
	MStatus compute(const MPlug& plug, MDataBlock& data) override;
	static  void* creator();
	static  MStatus initialize();

	static MObject	outputMesh;
	static MTypeId	id;

	static MObject deltaTime;
	static MObject numGrows;
	//static MObject stepSize;
	//static MObject grammar;
};