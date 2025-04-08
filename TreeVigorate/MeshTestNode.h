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

#include <vector>
#include <map>
#include <stack>
#include <unordered_set>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

#define McheckErr(stat,msg)         \
    if ( MS::kSuccess != stat ) {   \
        cerr << msg;                \
        return MS::kFailure;        \
    }

// Edge struct to store vertex indices
struct Edge {
	size_t v1, v2;

	// constructor
	Edge(size_t a, size_t b) : v1(std::min(a, b)), v2(std::max(a, b)) {}
	// equality testing
	bool operator==(const Edge& other) const {
		return v1 == other.v1 && v2 == other.v2;
	}
};

// Hash function for Edges
namespace std {
	template<> struct hash<Edge> {
		size_t operator()(const Edge& e) const {
			return hash<size_t>()(e.v1) ^ (hash<size_t>()(e.v2) << 1);
		}
	};
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
	/// Constructs a triangulation of points on a 
	/// plane, with edges < maxEdge removed.
	/// </summary>
	/// <param name="points"> the input points for the plane mesh</param>
	/// <param name="maxEdge"> max edge length</param>
	/// <returns> returns a vector indices for the triangles </returns>
	std::vector<int> getPlaneTriangleIdx(std::vector<glm::vec2> points, float maxEdge);

	/// <summary>
	/// Adds the triangles to Maya, with pts as vectors,
	/// and tris as the indices of the triangles referring to the
	/// vectors in pts.
	/// </summary>
	/// <param name="tris"> the triangle indices</param>
	/// <param name="pts"> the vector pts</param>
	/// <param name="points"> Maya point array</param>
	/// <param name="faceCounts"> Maya face count int array</param>
	/// <param name="faceConns"> Maya face connections int array</param>
	void createTris(std::vector<int>& tris, std::vector<glm::vec2>& pts,
		MPointArray& points, MIntArray& faceCounts, MIntArray& faceConns);

	static MObject	outputMesh;
	static MTypeId	id;

	// Maximum edge length before we chop it
	static MObject maxLen;
	// number of points on the mesh plane
	static MObject numPts;
	// Alpha is smth for edge mesh detection
	static MObject alpha;
};