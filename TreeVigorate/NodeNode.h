#pragma once
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
#include <maya/MTypeId.h>

#define McheckErr(stat,msg)         \
    if ( MS::kSuccess != stat ) {   \
        cerr << msg;                \
        return MS::kFailure;        \
    }

class NodeNode : public MPxNode {
private:
	/// <summary>
	/// Creates cube centered around pos
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="size"></param>
	/// <param name="points"></param>
	/// <param name="faceCounts"></param>
	/// <param name="faceConnects"></param>
	void createCube(glm::vec3 pos, float size,
			MPointArray& points,
			MIntArray& faceCounts,
			MIntArray& faceConnects);
public:
	NodeNode() {};
	~NodeNode() override {};
	/// <summary>
	/// Default maya function called to compute the output mesh
	/// </summary>
	/// <param name="plug"></param>
	/// <param name="data"></param>
	/// <returns></returns>
	MStatus compute(const MPlug& plug, MDataBlock& data) override;
	static  void* creator();
	static  MStatus initialize();

	static MObject	outputMesh;
	static MTypeId	id;

	// World Location of this node
	static MObject pos;
	// ID of this node in the tree
	static MObject nodeID;
	// Allocated vigor
	static MObject vigor;
	// Added rotation
	static MObject rot;
	// used to make node visible or not
	static MObject isVisible;
};