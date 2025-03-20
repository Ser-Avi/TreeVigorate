/*
    Tables and conventions from
    http://paulbourke.net/geometry/polygonise/
*/

#pragma once

#include "ecosyslab_export.h"
#include <glm/glm.hpp>
#include <vector>
#include <functional>

struct Vertex
{
    glm::vec3 m_position = glm::vec3(0.0f);
    float m_positionPadding = 0.0f;
    glm::vec3 m_normal = glm::vec3(0.0f);
    float m_normalPadding = 0.0f;
    glm::vec3 m_tangent = glm::vec3(0.0f);
    float m_tangentPadding = 0.0f;
    glm::vec4 m_color = glm::vec4(1.0f);
    glm::vec2 m_texCoord = glm::vec2(0.0f);
    glm::vec2 m_texCoordPadding = glm::vec2(0.0f);
};


// using namespace UniEngine;
namespace EcoSysLab
{
    struct MarchingCubeCell
    {
        glm::vec3 m_vertex[8];
        float m_value[8];
    };

    class MarchingCubes
    {
    public:

        /// m_edgeToVertices[i] = {a, b} => edge i joins vertices a and b
        static std::vector<std::pair<int, int>> m_edgeToVertices;

        /// m_edgeTable[i] is a 12 bit number; i is a cubeIndex
        /// m_edgeTable[i][j] = 1 if isosurface intersects edge j for cubeIndex i
        static int m_edgeTable[256];

        /// m_triangleTable[i] is a list of edges forming triangles for cubeIndex i
        static int m_triangleTable[256][16];
#pragma endregion
        /// Get triangles of a single cell
        static void TriangulateCell(MarchingCubeCell& cell, float isovalue, std::vector<Vertex>& vertices);
        
        /// Triangulate a scalar field represented by `scalarFunction`. `isovalue` should be used for isovalue computation
        static void TriangulateField(const glm::vec3 &center, const std::function<float(const glm::vec3 &samplePoint)>& sampleFunction, float isovalue, float cellSize, const std::vector<glm::vec3>& testingCells, 
            std::vector<Vertex>& vertices, std::vector<unsigned>& indices, bool removeDuplicate, int smoothMeshIteration);
    };
}