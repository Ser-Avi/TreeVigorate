#include "MeshTestNode.h"

MObject MeshTestNode::outputMesh;
MTypeId MeshTestNode::id(0x80500);

MObject MeshTestNode::maxLen;
MObject MeshTestNode::numPts;
MObject MeshTestNode::alpha;

void* MeshTestNode::creator()
{
	return new MeshTestNode;
}

MStatus MeshTestNode::initialize()
{
	MFnUnitAttribute unitAttr;
	MFnTypedAttribute typedAttr;
	MFnNumericAttribute numAttr;

	MStatus returnStatus;

	MeshTestNode::maxLen = numAttr.create("maxLen", "ml", MFnNumericData::kDouble, 10.0, &returnStatus);
	McheckErr(returnStatus, "Error creating maxLen attribute \n");

	MeshTestNode::numPts = numAttr.create("numPts", "np", MFnNumericData::kInt, 5, &returnStatus);
	McheckErr(returnStatus, "Error creating numPts attribute \n");

	MeshTestNode::alpha = numAttr.create("alpha", "a", MFnNumericData::kDouble, 0.5, &returnStatus);
	McheckErr(returnStatus, "Error creating alpha attribute \n");

	MeshTestNode::outputMesh = typedAttr.create("outputMesh", "out",
		MFnData::kMesh,
		MObject::kNullObj,
		&returnStatus);
	McheckErr(returnStatus, "ERROR creating TreeNode output attribute\n");
	/*typedAttr.setStorable(false);*/

	returnStatus = addAttribute(MeshTestNode::outputMesh);
	McheckErr(returnStatus, "ERROR adding outputMesh attribute\n");

	returnStatus = addAttribute(MeshTestNode::maxLen);
	McheckErr(returnStatus, "ERROR adding maxlen attribute\n");

	returnStatus = addAttribute(MeshTestNode::numPts);
	McheckErr(returnStatus, "ERROR adding numpts attribute\n");

	returnStatus = addAttribute(MeshTestNode::alpha);
	McheckErr(returnStatus, "ERROR adding numpts attribute\n");

	returnStatus = attributeAffects(MeshTestNode::numPts,
		MeshTestNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	returnStatus = attributeAffects(MeshTestNode::alpha,
		MeshTestNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	returnStatus = attributeAffects(MeshTestNode::maxLen,
		MeshTestNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	return MS::kSuccess;
}

void createMesh(std::vector<glm::vec2>& pts, MPointArray& points, MIntArray& faceCounts, MIntArray& faceConns) {
	int numPs = pts.size();
	for (int i = 0; i < numPs; ++i) {
		glm::vec2 p = pts[i];
		points.append(MPoint(p.x, 0, p.y));
		//points.append(MPoint(p.x, 1, p.y));
	}
	//for (int i = 0; i < numPs; ++i) {
	//	int next = (i + 1) % numPs;
	//	faceCounts.append(4);
	//	// Front vertices are even indices
	//	// Back vertices are odd indices
	//	faceConns.append(2 * i);
	//	faceConns.append(2 * next);
	//	faceConns.append(2 * next + 1);
	//	faceConns.append(2 * i + 1);
	//}
	// Add front face (original polygon)
	faceCounts.append(numPs);
	for (int i = 0; i < numPs; ++i) {
		faceConns.append(i); // Front vertices in order
	}

	// Add back face (reverse order for correct normal)
	//faceCounts.append(numPs);
	//for (int i = numPs - 1; i >= 0; --i) {
	//	faceConns.append(2 * i); // Back vertices in reverse order
	//}
}

MStatus MeshTestNode::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus returnStatus;

	if (plug == outputMesh) {
		// Get vars
		double mLen = data.inputValue(maxLen).asDouble();
		int numPs = data.inputValue(numPts).asInt();
		double alp = data.inputValue(alpha).asDouble();

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

		std::vector<glm::vec2> pts;

		//float theta = 3.1415926535 * 2.f / (float)numPs;
		float rad = 5;
		for (int i = 0; i < numPs; ++i) {
			//float angle = theta * i;
			//pts.push_back(glm::vec2(cos(angle), sin(angle)) * rad);
			//pts.push_back(glm::diskRand(rad));
			if (i % 2 == 0) {
				pts.push_back(glm::vec2(-10 + i * 0.2, i * 0.5));
			}
			else {
				pts.push_back(glm::vec2(10 + i * 0.2, i * 0.5));
			}
		}

		std::vector<std::vector<glm::vec2>> boundaryPoints = getBoundaryPts(pts, mLen);


		/*

		// removing long edges
		std::vector<std::pair<CGAL_Point, CGAL_Point>> edges;
		for (auto edge = dt.finite_edges_begin(); edge != dt.finite_edges_end(); ++edge) {
			auto seg = dt.segment(edge); // Get edge as a segment
			if (CGAL::sqrt(seg.squared_length()) <= mLen) {
				edges.emplace_back(seg.source(), seg.target());
			}
			else {
				MGlobal::displayInfo("Edge Cut");
			}
		}

		// getting bounding edges
		Alpha_shape alphaShape(dt);
		//double alpha = 0.5; // Adjust for concavity (smaller = more concave)
		//alphaShape.set_alpha(alp);
		alphaShape.set_alpha(*alphaShape.find_optimal_alpha(1));
		MGlobal::displayInfo("Alpha shape generated");

		// clearing vectors that we'll reuse here
		cgalPts.clear();
		pts.clear();

		//std::vector<Alpha_shape::Edge> boundaryEdges;
		for (auto edge = alphaShape.alpha_shape_edges_begin();
			edge != alphaShape.alpha_shape_edges_end();
			++edge) {
			if (alphaShape.classify(*edge) == Alpha_shape::REGULAR) {
				//boundaryEdges.push_back(*edge);
				MGlobal::displayInfo("in if");
				
				// getting the point information
				// Get the face containing the edge
				Alpha_shape::Face_handle face = edge->first;
				int edgeIndex = edge->second; // 0, 1, or 2

				// Get the first vertex on the edge (since we know this is a loop)
				const K::Point_2& p1 = face->vertex((edgeIndex + 1) % 3)->point();
				//const K::Point_2& p2 = face->vertex((edgeIndex + 2) % 3)->point();
				//edgeVertices.emplace_back(p1, p2);
				cgalPts.push_back(p1);
			}
			else {
				MGlobal::displayInfo("failed");
			}
		}
		*/
		MGlobal::displayInfo("Mesh generated");

		for (int i = 0; i < boundaryPoints.size(); ++i) {
			createMesh(boundaryPoints[i], points, faceCounts, faceConns);
		}

		//createMesh(pts, points, faceCounts, faceConns);

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

std::vector<CGAL_Point> MeshTestNode::convertToCGAL(std::vector<glm::vec2>& pts) {
	std::vector<CGAL_Point> cgalPs;
	for (const glm::vec2& p : pts) {
		cgalPs.emplace_back(p.x, p.y);
	}
	return cgalPs;
}
std::vector<glm::vec2> MeshTestNode::convertToGLM(std::vector<CGAL_Point>& pts) {
	std::vector<glm::vec2> glmPs;
	for (const CGAL_Point& p : pts) {
		glmPs.emplace_back(glm::vec2(p[0], p[1]));
	}
	return glmPs;
}

std::vector<std::vector<glm::vec2>> MeshTestNode::getBoundaryPts(std::vector<glm::vec2> points, float maxEdge) {
	// I.
	// first we convert from glm to cgal points
	std::vector<CGAL_Point> cgalPts = convertToCGAL(points);
	std::vector<std::pair<CGAL_Point, size_t>> indexedPts;			// keeping track of point indices
	for (size_t i = 0; i < cgalPts.size(); ++i) {
		indexedPts.emplace_back(cgalPts[i], i);  // Store with index
	}
	std::vector<std::vector<glm::vec2>> finalPts;	// we will return this at the end
	
	// II.
	// delaunay triangulate points
	Delaunay dt;
	dt.insert(indexedPts.begin(), indexedPts.end());
	
	// III.
	// cull long edges
	std::unordered_set<Edge> keptEdges;
	bool everyShort = true; // keeps track if every edge in the triangulation was valid -> we can skip later steps
	// loop over each triangle
	for (auto tri = dt.finite_faces_begin(); tri != dt.finite_faces_end(); ++tri) {
		bool allShort = true;
		std::vector<Edge> triEdges;

		// check each edge
		for (int i = 0; i < 3; ++i) {
			auto v1 = tri->vertex((i + 1) % 3);
			auto v2 = tri->vertex((i + 2) % 3);

			// making sure we have the right indices (had bug here where it wasn't)
			if (v1->info() >= points.size() || v2->info() >= points.size()) {
				throw std::runtime_error("Invalid vertex index detected");
			}

			// if too long, cull and mark bools false
			if (CGAL::sqrt(CGAL::squared_distance(v1->point(), v2->point())) > maxEdge) {
				allShort = false;
				everyShort = false;
				break;
			}
			triEdges.emplace_back(v1->info(), v2->info());
		}
		// if every edge on this triangle is short, we can keep it
		if (allShort) {
			for (const auto& edge : triEdges) {
				keptEdges.insert(edge);
			}
		}
	}

	// if we didn't cull any edges, we can simply use our base Delaunay mesh
	if (everyShort) {
		Delaunay::Vertex_circulator vc = dt.incident_vertices(dt.infinite_vertex()),
			done(vc);
		cgalPts.clear();
		if (vc != nullptr) {
			do {
				cgalPts.push_back(vc->point());
			} while (++vc != done);
		}

		points = convertToGLM(cgalPts);
		finalPts.push_back(points);
		return  finalPts;
	}

	// IV.
	// get new boundary edges
	std::unordered_set<Edge> boundaryEdges;
	// if we see an edge once -> boundary. twice -> inner
	for (const auto& edge : keptEdges) {
		// if we've seen this edge, remove it
		if (boundaryEdges.find(edge) != boundaryEdges.end()) {
			boundaryEdges.erase(edge);
		}
		// if we haven't we add it
		else {
			boundaryEdges.insert(edge);
		}
	}

	// V.
	// Find which components are connected
	// Now find connected components of boundary edges
	std::unordered_map<size_t, std::vector<size_t>> adjacency;
	// first we build the adjacency list
	for (const auto& edge : boundaryEdges) {
		adjacency[edge.v1].push_back(edge.v2);
		adjacency[edge.v2].push_back(edge.v1);
	}

	// use DFS to find connected components
	std::unordered_set<size_t> visited;
	for (const auto& pair : adjacency) {
		size_t start = pair.first;				// start of edge
		if (visited.find(start) != visited.end()) continue;	// if we already visited this, continue

		std::vector<size_t> component;
		std::vector<size_t> stack;
		stack.push_back(start);
		visited.insert(start);
		// basic BFS algo
		while (!stack.empty()) {
			size_t current = stack.back();
			stack.pop_back();
			component.push_back(current);

			for (size_t neighbor : adjacency[current]) {
				if (visited.find(neighbor) == visited.end()) {
					visited.insert(neighbor);
					stack.push_back(neighbor);
				}
			}
		}

	// VI.
	// Finally points to use in Maya mesh drawing, forming boundary loops
		if (!component.empty()) {
			std::vector<glm::vec2> boundaryPts;

			// component adjacency list
			std::unordered_map<size_t, std::vector<size_t>> compAdj;
			for (const auto& edge : boundaryEdges) {
				if (std::find(component.begin(), component.end(), edge.v1) != component.end() &&
					std::find(component.begin(), component.end(), edge.v2) != component.end()) {
					compAdj[edge.v1].push_back(edge.v2);
					compAdj[edge.v2].push_back(edge.v1);
				}
			}

			if (!compAdj.empty()) {
				size_t current = component[0];
				size_t prev = current;
				boundaryPts.push_back(points[current]);

				// Traverse the boundary in order
				size_t next = compAdj[current][0];
				while (next != current && boundaryPts.size() < component.size()) {
					boundaryPts.push_back(points[next]);
					auto& neighbors = compAdj[next];
					size_t temp = next;
					if (neighbors[0] != prev) {
						next = neighbors[0];
					}
					else if (neighbors.size() > 1) {
						next = neighbors[1];
					}
					else {
						break; // shouldn't happen for closed loops
					}
					prev = temp;
				}

				finalPts.push_back(boundaryPts);
			}
		}
	}
	return finalPts;
}