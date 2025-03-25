#ifndef cylinder_H_
#define cylinder_H_

#include <maya/MPoint.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#define M_PI       3.14159265358979323846   // pi

class CylinderMesh
{
public:
    CylinderMesh(const MPoint& start, const MPoint& end, double r = 0.25);
    ~CylinderMesh();

    void getMesh(
        MPointArray& points, 
        MIntArray& faceCounts, 
        MIntArray& faceConnects);
    
    void appendToMesh(
        MPointArray& points, 
        MIntArray& faceCounts, 
        MIntArray& faceConnects,
        float sRad, float eRad, glm::quat sRot,
        glm::quat eRot);

protected:
    void transform(MPointArray& points, MVectorArray& normals);
    void transformNew(MPointArray& cpoints, float sRad, float eRad, glm::quat sRot, glm::quat eRot);
    MPoint mStart;
    MPoint mEnd;
    double r;

    // Creates a unit cylinder from (0,0,0) to (1,0,0) with radius r
    static void initCylinderMesh(double r);
    static MPointArray gPoints;
    static MVectorArray gNormals;
    static MIntArray gFaceCounts;
    static MIntArray gFaceConnects;
};

#endif