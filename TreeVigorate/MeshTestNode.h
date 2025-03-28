#pragma once

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
#include <maya/MGlobal.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Alpha_shape_face_base_2.h>
#include <CGAL/Alpha_shape_vertex_base_2.h>
#include <CGAL/Alpha_shape_2.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 CGAL_Point;
typedef CGAL::Alpha_shape_vertex_base_2<K> Vb;
typedef CGAL::Alpha_shape_face_base_2<K>   Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Delaunay_triangulation_2<K, Tds> Delaunay;
typedef CGAL::Alpha_shape_2<Delaunay> Alpha_shape;

#define McheckErr(stat,msg)         \
    if ( MS::kSuccess != stat ) {   \
        cerr << msg;                \
        return MS::kFailure;        \
    }

class MeshTestNode : public MPxNode {
public:
	MeshTestNode() {};
	~MeshTestNode() override {};
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
	/// Converts glm::vec2s to CGAL points
	/// </summary>
	/// <param name="pts"></param>
	/// <returns></returns>
	std::vector<CGAL_Point> convertToCGAL(std::vector<glm::vec2>& pts);
	/// <summary>
	/// Converts CGAL points to glm::vec2s
	/// </summary>
	/// <param name="pts"></param>
	/// <returns></returns>
	std::vector<glm::vec2> convertToGLM(std::vector<CGAL_Point>& pts);

	static MObject	outputMesh;
	static MTypeId	id;

	// Maximum edge length before we chop it
	static MObject maxLen;
	// number of points on the mesh plane
	static MObject numPts;
	// Alpha is smth for edge mesh detection
	static MObject alpha;
};