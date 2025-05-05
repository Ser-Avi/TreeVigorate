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
MObject TreeNode::sunDir;
MObject TreeNode::growTime;
MObject TreeNode::segments;


MObject TreeNode::numNodes;
MObject TreeNode::selectedNode;
MObject TreeNode::numSubGrows;
MObject TreeNode::growNode;
MObject TreeNode::pruneNode;
MObject TreeNode::outputNodeMesh;

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
	numAttr.setNiceNameOverride("Growth rate");

	TreeNode::numGrows = numAttr.create("numGrows", "ng", MFnNumericData::kInt, 0, &returnStatus);
	McheckErr(returnStatus, "Error creating TreeNode numGrows attribute \n");
	numAttr.setNiceNameOverride("Growth amount");

	TreeNode::radius = numAttr.create("radius", "r", MFnNumericData::kDouble, 1.0, &returnStatus);
	McheckErr(returnStatus, "Error creatin radius attribute \n");
	numAttr.setNiceNameOverride("Radius multiplier");

	TreeNode::treeDataFile = typedAttr.create("treeDataFile", "f", MFnData::kString, MObject::kNullObj, &returnStatus);
	McheckErr(returnStatus, "Error creating treeDataFile attribute \n");
	typedAttr.setHidden(true);

	TreeNode::makeGrow = numAttr.create("makeGrow", "mg", MFnNumericData::kBoolean, false, &returnStatus);
	McheckErr(returnStatus, "Error creating makeGrow attribute\n");

	TreeNode::segments = numAttr.create("segments", "s", MFnNumericData::kInt, 1, &returnStatus);
	McheckErr(returnStatus, "Error creating segments attribute\n");

	TreeNode::sunDir = numAttr.create("sunDir", "sd", MFnNumericData::k3Double, 0, &returnStatus);
	numAttr.setNiceNameOverride("Light Direction");
	numAttr.setDefault(0.0, 1.0, 0.0);
	McheckErr(returnStatus, "Error creating sunDir attrib\n");

	TreeNode::growTime = numAttr.create("growTime", "gt", MFnNumericData::kDouble, 0, &returnStatus);
	numAttr.setStorable(true);
	numAttr.setWritable(true);
	numAttr.setReadable(true);
	numAttr.setHidden(false);
	numAttr.setKeyable(false);
	McheckErr(returnStatus, "Error creating growTime attrib\n");

	TreeNode::outputMesh = typedAttr.create("outputMesh", "out",
		MFnData::kMesh,
		MObject::kNullObj,
		&returnStatus);
	McheckErr(returnStatus, "ERROR creating TreeNode output attribute\n");
	/*typedAttr.setStorable(false);*/
	typedAttr.setHidden(true);

	// NODE SPECIFIC ATTRIBUTES
	TreeNode::numNodes = numAttr.create("numNodes", "nn", MFnNumericData::kInt, 0, &returnStatus);
	numAttr.setStorable(true);
	numAttr.setWritable(true);
	numAttr.setReadable(true);
	numAttr.setKeyable(false);
	TreeNode::selectedNode = numAttr.create("selectedNode", "sn", MFnNumericData::kInt, -1, &returnStatus);
	TreeNode::numSubGrows = numAttr.create("numSubGrows", "nsg", MFnNumericData::kInt, 0, &returnStatus);
	McheckErr(returnStatus, "Error creating selectedVigor attrib\n");
	TreeNode::outputNodeMesh = typedAttr.create("outputNodeMesh", "outNode",
		MFnData::kMesh,
		MObject::kNullObj,
		&returnStatus);
	typedAttr.setHidden(true);
	TreeNode::growNode = numAttr.create("growNode", "gn", MFnNumericData::kBoolean, false, &returnStatus);
	McheckErr(returnStatus, "Error creating growNode attribute\n");
	TreeNode::pruneNode = numAttr.create("pruneNode", "pn", MFnNumericData::kBoolean, false, &returnStatus);
	McheckErr(returnStatus, "Error creating pruneNode attribute\n");

	returnStatus = addAttribute(TreeNode::outputMesh);
	McheckErr(returnStatus, "ERROR adding outputMesh attribute\n");

	returnStatus = addAttribute(TreeNode::deltaTime);
	McheckErr(returnStatus, "ERROR adding deltaTime attribute\n");

	returnStatus = addAttribute(TreeNode::numGrows);
	McheckErr(returnStatus, "ERROR adding numGrows attribute\n");

	returnStatus = addAttribute(TreeNode::radius);
	McheckErr(returnStatus, "ERROR adding radius attribute\n");

	returnStatus = addAttribute(TreeNode::segments);
	McheckErr(returnStatus, "ERROR adding segments attribute\n");

	returnStatus = addAttribute(TreeNode::treeDataFile);
	McheckErr(returnStatus, "ERROR adding treeDataFile attribute\n");

	returnStatus = addAttribute(TreeNode::makeGrow);
	McheckErr(returnStatus, "ERROR adding makeGrow attribute\n");

	returnStatus = attributeAffects(TreeNode::makeGrow,
		TreeNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");
	
	returnStatus = attributeAffects(TreeNode::radius,
		TreeNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");


	returnStatus = addAttribute(TreeNode::sunDir);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	returnStatus = addAttribute(TreeNode::growTime);
	McheckErr(returnStatus, "ERROR adding growTime attribute\n");

	// NODE ATTRIBUTES
	returnStatus = addAttribute(TreeNode::numNodes);
	returnStatus = addAttribute(TreeNode::selectedNode);
	returnStatus = addAttribute(TreeNode::numSubGrows);
	McheckErr(returnStatus, "ERROR adding vigor attribute\n");
	returnStatus = addAttribute(TreeNode::growNode);
	returnStatus = addAttribute(TreeNode::outputNodeMesh);
	returnStatus = addAttribute(TreeNode::pruneNode);

	returnStatus = attributeAffects(TreeNode::selectedNode,
		TreeNode::outputNodeMesh);
	returnStatus = attributeAffects(TreeNode::growNode,
		TreeNode::outputMesh);
	returnStatus = attributeAffects(TreeNode::pruneNode,
		TreeNode::outputMesh);
	returnStatus = attributeAffects(TreeNode::segments,
		TreeNode::outputMesh);
	// everything that affects the outputMesh, needs to also affect the outputnodeMesh, as any change in the above
	// will affect the highlighted node. Could this just be making outputMesh affect outputnodeMesh?
	// No, because Maya.
	returnStatus = attributeAffects(TreeNode::growNode,
		TreeNode::outputNodeMesh);
	returnStatus = attributeAffects(TreeNode::radius,
		TreeNode::outputNodeMesh);
	returnStatus = attributeAffects(TreeNode::makeGrow,
		TreeNode::outputNodeMesh);
	returnStatus = attributeAffects(TreeNode::pruneNode,
		TreeNode::outputNodeMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	return MS::kSuccess;
}

void TreeNode::appendLeavesToMesh(ShootSkeleton& skeleton, StrandManager strandManager, double rad)
{
	for (auto& node : skeleton.RefRawNodes()) {
		for (auto& leaf : node.m_data.m_leaves) {

		}
	}
}

MStatus TreeNode::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus returnStatus;

	if (plug == outputMesh) {
		// Get vars
		double fDTime = data.inputValue(deltaTime).asDouble();
		int nGrows = data.inputValue(numGrows).asInt();
		float r = data.inputValue(radius).asDouble();
		float seg = data.inputValue(segments).asInt();
		double3& sunDirVal = data.inputValue(sunDir).asDouble3();
		glm::vec3 sunVec = glm::normalize(glm::vec3(sunDirVal[0], sunDirVal[1], sunDirVal[2]));
		bool isNodeChanged = data.inputValue(growNode).asBool();
		bool isPruning = data.inputValue(pruneNode).asBool();

		// For keeping track of total grow time
		MDataHandle growTimeHandle = data.outputValue(growTime);
		double currGrowTime = growTimeHandle.asDouble();

		if (sunVec != treeModel.lightDir) {
			treeModel.lightDir = sunVec;
			MGlobal::displayInfo("Light Direction Changed");
		}

		// Initializing tree params
		if (!treeParams.isInit) {
			MString treeInfo = data.inputValue(treeDataFile).asString();
			treeParams.isInit = true;
			InitializeMVars(treeInfo.asChar(), treeParams.sm, treeParams.cm, treeParams.rgc, treeParams.sgc);
		}

		// create output object
		MDataHandle outputHandle = data.outputValue(outputMesh, &returnStatus);
		McheckErr(returnStatus, "ERROR getting polygon data handle\n");

		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);
		McheckErr(returnStatus, "ERROR creating outputData");

		MPointArray points;
		MIntArray faceCounts;
		MIntArray faceConns;

		// Growing tree
		if (!isNodeChanged && abs(r - prevRad) <= 0.01 && !isPruning && seg == prevSeg) {
			MGlobal::displayInfo("Abt to Grow, stand back!!");
			// if we aren't changing node vigor, we grow normally
			for (int i = 0; i < nGrows; ++i) {
				/*for (int j = 0; j < nodes; ++j) {
					auto& snode = treeModel.RefShootSkeleton().RefNode(j);
				}*/
				bool didGrow = treeModel.Grow(fDTime, glm::mat4(), treeParams.sm, treeParams.cm, treeParams.rgc, treeParams.sgc);
				/*MGlobal::displayInfo("Growth successful, iteration: ");
				MGlobal::displayInfo(std::to_string(i + 1).c_str());

				MGlobal::displayInfo("Shoot nodes: ");
				int nodes = treeModel.RefShootSkeleton().RefSortedNodeList().size();
				MGlobal::displayInfo(MString(std::to_string(nodes).c_str()));

				int flows = treeModel.RefShootSkeleton().RefSortedFlowList().size();
				MGlobal::displayInfo("Flows: ");
				MGlobal::displayInfo(MString(std::to_string(flows).c_str()));*/

				// incrementing grow time
				currGrowTime += fDTime;

				// loading bar in MEL
				MString loadingBar = getLoadBar(i, nGrows);
				MGlobal::displayInfo(loadingBar);
			}
		}
		else if (isNodeChanged && !isPruning) {
			// else we only grow from this node
			int subGrowNum = data.inputValue(numSubGrows).asInt();

			int currNode = data.inputValue(selectedNode).asInt() - 1;
			if (currNode < 0) {
				MGlobal::displayError("Node Index Invalid");

				data.setClean(plug);
				return MS::kSuccess;
			}

			Skeleton skeleton = treeModel.RefShootSkeleton();
			NodeHandle currHandle = skeleton.RefSortedFlowList()[currNode];
			Flow currFlow = skeleton.RefFlow(currHandle);
			const NodeHandle first = currFlow.RefNodeHandles()[0];
			Node curr = skeleton.RefNode(first);
			//curr.m_data.m_vigorFlow.m_subTreeAllocatedVigor = currVig;

			// lets add an even amount of vigor to the buds on the first node
			/*int numBuds = curr.m_data.m_buds.size(); 
			float initialVig = 0;
			for (auto& bud : curr.m_data.m_buds) {
				if (bud.m_status != BudStatus::Died) {
					initialVig += bud.m_vigorSink.GetVigor();
				}
			}

			float newVig = currVig - initialVig;
			newVig /= (float)numBuds;

			for (auto& bud : curr.m_data.m_buds) {
				if (bud.m_status != BudStatus::Died) {
					bud.m_vigorSink.AddVigor(newVig);
				}
			}*/

			for (int i = 0; i < subGrowNum; ++i) {
				developSubtree(skeleton, first, sunVec);
				
				// loading bar in MEL
				MString loadingBar = getLoadBar(i, subGrowNum);
				MGlobal::displayInfo(loadingBar);
			}

			// reset bool to false
			MDataHandle boolHand = data.outputValue(growNode);
			boolHand.set(false);
			boolHand.setClean();
			MGlobal::displayInfo("finish nodegrow");
		} else if (isPruning) {
			int currNode = data.inputValue(selectedNode).asInt() - 1;
			if (currNode < 0) {
				MGlobal::displayError("Node Index Invalid");

				data.setClean(plug);
				return MS::kSuccess;
			}

			Skeleton skeleton = treeModel.RefShootSkeleton();
			NodeHandle currHandle = skeleton.RefSortedFlowList()[currNode];
			Flow currFlow = skeleton.RefFlow(currHandle);
			const NodeHandle first = currFlow.RefNodeHandles()[0];

			pruneSubtree(skeleton, first);

			// reset bool to false
			MDataHandle boolHand = data.outputValue(pruneNode);
			boolHand.set(false);
			boolHand.setClean();
			MGlobal::displayInfo("finished pruning");
		}
		//MGlobal::displayInfo("Rad:");
		//MGlobal::displayInfo(std::to_string(r).c_str());
		
		StrandManager strandManager;
		strandManager.generateParticlesForTree(treeModel.RefShootSkeleton(), 3, r);
		std::unordered_map<NodeHandle, DelauneyData> nodeToDelauneyData;
		//for now, draw each plane

		for (auto& handle : treeModel.RefShootSkeleton().RefRawNodes()) {
			std::unordered_map<NodeHandle, glm::vec3> positions = strandManager.getGlobalNodeParticlePositions(handle.GetHandle(), treeModel.RefShootSkeleton());
			//todo :: what should max edge actually be?
			if (false && positions.size() > 2) {
				DelauneyData delauneyData = strandManager.getPlaneTriangleIdx(handle.GetHandle(), 999999, points.length());
				//for (int i = 0; i < 5; ++i) {
				//strandManager.resolvePbd(treeModel.RefShootSkeleton(), handle.GetHandle(), delauneyData, r);
				//}

				//reassign delauney after pbd shifting
				delauneyData = strandManager.getPlaneTriangleIdx(handle.GetHandle(), 999999, points.length());
				nodeToDelauneyData[handle.GetHandle()] = delauneyData;
				

				positions = strandManager.getGlobalNodeParticlePositions(handle.GetHandle(), treeModel.RefShootSkeleton());

				for (auto& particle : strandManager.getNodeToParticlesMap()[handle.GetHandle()]) {
					auto& point = positions[particle.getIndex()];
					MPoint p(point.x, point.y, point.z);
				//	points.append(p);
				}

				if (handle.RefChildHandles().size() != 1 || true) {
					for (int i = 0; i < delauneyData.idx.size(); i += 3) {
					//	faceCounts.append(3);
					//	faceConns.append(delauneyData.idx[i]);
					//	faceConns.append(delauneyData.idx[i + 1]);
					//	faceConns.append(delauneyData.idx[i + 2]);
					}
				}
			}
		}

		//std::vector<int> bridgeIndices = strandManager.getBridgeTriangleIdx(treeModel.RefShootSkeleton(), nodeToDelauneyData);
		//for (int i = 0; i < bridgeIndices.size(); i += 3) {
		//	faceCounts.append(3);
		//	faceConns.append(bridgeIndices[i]);
		//	faceConns.append(bridgeIndices[i + 1]);
		//	faceConns.append(bridgeIndices[i + 2]);
		//}

		// Creating cylinders
		ShootSkeleton shoots = treeModel.RefShootSkeleton();
		if (shoots.RefSortedNodeList().size() != 0) {
			bool isAdd = appendNodeCylindersToMesh(points, faceCounts, faceConns, shoots, strandManager, r, seg);
		}

		// setting growTime to new val
		growTimeHandle.set(currGrowTime);
		growTimeHandle.setClean();

		// setting number of nodes
		MDataHandle nodeNumHandle = data.outputValue(numNodes);
		int flows = shoots.RefSortedFlowList().size();
		nodeNumHandle.set(flows);
		nodeNumHandle.setClean();
		/*MGlobal::displayInfo("Flows: ");
		MGlobal::displayInfo(MString(std::to_string(flows).c_str()));
		int nodes = shoots.RefSortedNodeList().size();
		MGlobal::displayInfo("Nodes: ");
		MGlobal::displayInfo(std::to_string(nodes).c_str());*/

		MFnMesh mesh;
		mesh.create(points.length(), faceCounts.length(), points, faceCounts, faceConns, newOutputData, &returnStatus);
		McheckErr(returnStatus, "ERROR creating new Mesh");

		// store rad and segments
		prevRad = r;
		prevSeg = seg;

		MGlobal::displayInfo("[##########] 100%");

		outputHandle.set(newOutputData);
		data.setClean(plug);
	}
	else if (plug == outputNodeMesh) {
		int currNode = data.inputValue(selectedNode).asInt() - 1;
		if (currNode < 0) {
			//MGlobal::displayError("Node Index Invalid");

			data.setClean(plug);
			return MS::kSuccess;
		}

		Skeleton skeleton = treeModel.RefShootSkeleton();
		NodeHandle currHandle = skeleton.RefSortedFlowList()[currNode];
		Flow currFlow = skeleton.RefFlow(currHandle);

		// the flow could be empty if we just pruned, so we create an empty mesh and return it
		if (currFlow.RefNodeHandles().size() == 0) {
			MPointArray points;
			MIntArray faceCounts;
			MIntArray faceConns;
			// create output object
			MDataHandle outputHandle = data.outputValue(outputNodeMesh, &returnStatus);
			McheckErr(returnStatus, "ERROR getting polygon data handle\n");
			MFnMeshData dataCreator;
			MObject newOutputData = dataCreator.create(&returnStatus);
			McheckErr(returnStatus, "ERROR creating outputData");

			MFnMesh mesh;
			mesh.create(points.length(), faceCounts.length(), points, faceCounts, faceConns, newOutputData, &returnStatus);
			McheckErr(returnStatus, "ERROR creating new Mesh");

			outputHandle.set(newOutputData);
			data.setClean(plug);
			return MStatus::kSuccess;
		}

		NodeHandle first = currFlow.RefNodeHandles()[0];
		Node curr = skeleton.RefNode(first);
		//double treeVig = curr.m_data.m_vigorFlow.m_subTreeAllocatedVigor;
		// the vigor is stored in the buds
		//double treeVig = 0;
		//for (auto& bud : curr.m_data.m_buds) {
		//	if (bud.m_status != BudStatus::Died) {
		//		treeVig += bud.m_vigorSink.GetVigor();
		//	}
		//}
		
		//float vigor = curr.m_data.m_buds[0].m_vigorSink.GetVigor();

		// highlighting selected flow
		float r = data.inputValue(radius).asDouble() * 2.2;
		glm::vec3 currPos = currFlow.m_info.m_globalStartPosition;
		glm::vec3 parentPos = currFlow.m_info.m_globalEndPosition;
		MPoint start(currPos[0], currPos[1], currPos[2]);
		MPoint end(parentPos[0], parentPos[1], parentPos[2]);
		glm::vec3 sDir;
		if (currFlow.GetParentHandle() >= 0) {
			sDir = currPos - skeleton.PeekFlow(currFlow.GetParentHandle()).m_info.m_globalStartPosition;
		}
		else {
			sDir = glm::vec3(0, 1, 0);
		}
		glm::vec3 eDir = parentPos - currPos;
		MPointArray points;
		MIntArray faceCounts;
		MIntArray faceConns;
		buildCylinderMesh(start, end, currFlow.m_info.m_startThickness * r, currFlow.m_info.m_endThickness * r, sDir, eDir,
			points, faceCounts, faceConns, true, true);

		// create output object
		MDataHandle outputHandle = data.outputValue(outputNodeMesh, &returnStatus);
		McheckErr(returnStatus, "ERROR getting polygon data handle\n");
		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);
		McheckErr(returnStatus, "ERROR creating outputData");

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

/// <summary>
/// Based on the cylinder class method
/// </summary>
void TreeNode::buildCylinderMesh(MPoint& start, MPoint& end, float sRad, float eRad, glm::vec3 sDir, glm::vec3 eDir,
	MPointArray& points, MIntArray& faceCounts, MIntArray& faceConns, bool drawTop, bool drawBot) {
	int startIndex = points.length();
	int numSlices = 10;
	float theta = 2 * M_PI / (float) numSlices;
	glm::quat sRot = glm::rotation(glm::vec3(0, 1.f, 0), sDir);
	glm::quat eRot = glm::rotation(glm::vec3(0, 1.f, 0), eDir);

	bool needFaceReverse = (end.y - start.y) > 0;
	
	// starting circle points
	for (int i = 0; i < numSlices; ++i) {
		glm::vec3 p(cos(theta * i), 0, sin(theta * i));
		p *= sRad;
		p = sRot * p;
		points.append(MPoint(p.x, p.y, p.z) + start);
	}
	// ending circle points
	for (int i = 0; i < numSlices; ++i) {
		glm::vec3 p(cos(theta * i), 0, sin(theta * i));
		p *= eRad;
		p = eRot * p;
		points.append(MPoint(p.x, p.y, p.z) + end);
	}
	// endcaps
	points.append(start);	// index: 2 * numslices + startIndex
	points.append(end);		// index: above + 1
	// setting endcap 1 indices
	for (int i = 0; i < numSlices && drawBot; ++i) {
		faceCounts.append(3);
		if (!needFaceReverse) {
			faceConns.append(2 * numSlices + startIndex);  // Center
			faceConns.append((i + 1) % numSlices + startIndex); 
			faceConns.append(i + startIndex);              
		}
		else {
			faceConns.append(2 * numSlices + startIndex);  // Center
			faceConns.append(i + startIndex);              
			faceConns.append((i + 1) % numSlices + startIndex); 
		}
	}
	// endcap 2 indices
	for (int i = 0; i < numSlices && drawTop; ++i) {
		faceCounts.append(3);
		if (needFaceReverse) {
			faceConns.append(2 * numSlices + 1 + startIndex);  // Center
			faceConns.append(numSlices + (i + 1) % numSlices + startIndex);
			faceConns.append(numSlices + i + startIndex);
		}
		else {
			faceConns.append(2 * numSlices + 1 + startIndex);  // Center
			faceConns.append(numSlices + i + startIndex);
			faceConns.append(numSlices + (i + 1) % numSlices + startIndex);
		}
	}
	// the middle indices
	for (int i = 0; i < numSlices; ++i) {
		faceCounts.append(4);
		int next = (i + 1) % numSlices;
		if (needFaceReverse) {
			faceConns.append(i + startIndex);                   // Start circle, current
			faceConns.append(numSlices + i + startIndex);       // End circle, current
			faceConns.append(numSlices + next + startIndex);  // End circle, next
			faceConns.append(next + startIndex);              // Start circle, next
		}
		else {
			faceConns.append(i + startIndex);                   // Start circle, current
			faceConns.append(next + startIndex);              // Start circle, next
			faceConns.append(numSlices + next + startIndex);  // End circle, next
			faceConns.append(numSlices + i + startIndex);       // End circle, current
		}
	}
}

bool TreeNode::appendNodeCylindersToMesh(MPointArray& points, MIntArray& faceCounts, MIntArray& faceConns, ShootSkeleton& skeleton, StrandManager& strandManager, double radius, int segments) {
#define FLOW true;
#if FLOW
	// First we get all end nodes
	std::unordered_set<NodeHandle> endNodeHandles;
	for (const auto& fh: skeleton.RefSortedFlowList()) {
		const auto& flow = skeleton.RefFlow(fh);
		if (flow.RefChildHandles().size() == 0) {
			if (flow.RefNodeHandles().size() > 0) {
				endNodeHandles.insert(flow.RefNodeHandles()[flow.RefNodeHandles().size() - 1]);
			}
			else if (flow.GetParentHandle() >= 0) {
				const auto& parent = skeleton.RefFlow(flow.GetParentHandle());
				endNodeHandles.insert(parent.RefNodeHandles()[parent.RefNodeHandles().size() - 1]);
			}
		}
	}

	// Then, for each particle in each end node
	for (const auto& nh : endNodeHandles) {
		//auto& currNode = skeleton.RefNode(nh);
		const auto& particles = strandManager.getNodeToParticlesMap();
		for (const auto& particle : particles.at(nh)) {
			auto currFlowHand = skeleton.PeekNode(nh).GetFlowHandle();
			auto currFlow = skeleton.PeekFlow(currFlowHand);
			auto nextFlow = currFlow.GetParentHandle() > -1 ? skeleton.PeekFlow(currFlow.GetParentHandle()) : currFlow;
			auto prevFlow = currFlow;
			do {
				auto currNodeHand = currFlow.RefNodeHandles()[currFlow.RefNodeHandles().size() - 1];
				auto currNode = skeleton.PeekNode(currNodeHand);
				auto nextNodeHand = currFlow.GetParentHandle() > -1 ? nextFlow.RefNodeHandles()[nextFlow.RefNodeHandles().size() - 1] : currFlow.RefNodeHandles()[0];
				auto nextNode = skeleton.PeekNode(nextNodeHand);
				auto prevNodeHand = prevFlow.RefNodeHandles()[prevFlow.RefNodeHandles().size() - 1];
				auto prevNode = skeleton.PeekNode(prevNodeHand);
				auto nextnextFlowHand = nextFlow.GetParentHandle();


				std::unordered_map<int, glm::vec3> thisNodePositions = strandManager.getGlobalNodeParticlePositions(currNodeHand, skeleton);
				glm::vec3 b0 = thisNodePositions[particle.getIndex()];
				std::unordered_map<int, glm::vec3> nextNodePositions = strandManager.getGlobalNodeParticlePositions(nextNodeHand, skeleton);
				glm::vec3 b3 = nextNodePositions[particle.getIndex()];

				std::unordered_map<int, glm::vec3> prevNodePositions = strandManager.getGlobalNodeParticlePositions(prevNodeHand, skeleton);
				glm::vec3 prevPos = prevNodePositions[particle.getIndex()];
				glm::vec3 s0 = (b3 - prevPos) * 0.5f;
				glm::vec3 b1 = b0 + (1.f / 3.f) * s0;
				
				glm::vec3 s1;
				if (nextnextFlowHand != -1) {
					auto nextnextFlow = skeleton.PeekFlow(nextnextFlowHand);
					auto nextnextNodeHand = nextnextFlow.RefNodeHandles()[nextnextFlow.RefNodeHandles().size() - 1];
					auto nextnextNode = skeleton.PeekNode(nextnextNodeHand);
					std::unordered_map<int, glm::vec3> nextnextNodePositions = strandManager.getGlobalNodeParticlePositions(nextnextNodeHand, skeleton);
					glm::vec3 nextnextPos = nextnextNodePositions[particle.getIndex()];
					s1 = (nextnextPos - b0) * 0.5f;
				}
				else {
					glm::vec3 b3tob0 = b3 - b0;
					s1 = glm::vec3(0, b3tob0.y / 5.f, 0);
				}
				glm::vec3 b2 = b3 - (1.f / 3.f) * s1;


				// Drawing cylinders
				glm::vec3 sDir;
				sDir = glm::vec3(0, 1, 0);
				MPoint start(b0[0], b0[1], b0[2]);
				MPoint end(b3[0], b3[1], b3[2]);

				//	glm::vec3 eDir = parentPos - currPos;
				//buildCylinderMesh(start, end, currNode.m_info.m_thickness * radius, nextNode.m_info.m_thickness * radius,
				//	b3 - b0, b3 - b0, points, faceCounts, faceConns, (currNode.RefChildHandles().size() == 0), false);

				int segMins = glm::min(segments, (int)currFlow.RefNodeHandles().size());
				for (int seg = 0; seg < segMins; ++seg) {
					float uneg1 = ((seg - 1) / (double)segMins);
					float u3 = ((seg + 2) / (double)segMins);

					float u1 = seg / (double)segMins;
					float u2 = (seg + 1) / (double)segMins;

					glm::vec3 posneg1 =
						b0 * (float)glm::pow((1 - uneg1), 3)
						+ b1 * 3.f * uneg1 * (float)glm::pow((1 - uneg1), 2)
						+ b2 * 3.f * (float)glm::pow((uneg1), 2) * (1 - uneg1)
						+ b3 * (float)glm::pow((uneg1), 3);


					glm::vec3 pos1 = 
						b0 * (float) glm::pow((1 - u1), 3) 
						+ b1 * 3.f* u1 * (float)glm::pow((1 - u1), 2) 
						+ b2 * 3.f * (float)glm::pow((u1), 2) * (1 - u1)
						+ b3 * (float)glm::pow((u1), 3);

					glm::vec3 pos2 =
						b0 * (float)glm::pow((1 - u2), 3)
						+ b1 * 3.f * u2 * (float)glm::pow((1 - u2), 2)
						+ b2 * 3.f * (float)glm::pow((u2), 2) * (1 - u2)
						+ b3 * (float)glm::pow((u2), 3);

					glm::vec3 pos3 =
						b0 * (float)glm::pow((1 - u3), 3)
						+ b1 * 3.f * u3 * (float)glm::pow((1 - u3), 2)
						+ b2 * 3.f * (float)glm::pow((u3), 2) * (1 - u3)
						+ b3 * (float)glm::pow((u3), 3);

					MPoint start(pos1[0], pos1[1], pos1[2]);
					MPoint end(pos2[0], pos2[1], pos2[2]);


					float rad1 = (1 - u1) * currNode.m_info.m_thickness * radius + u1 * nextNode.m_info.m_thickness * radius;
					float rad2 = (1 - u2) * currNode.m_info.m_thickness * radius + u2 * nextNode.m_info.m_thickness * radius;

					buildCylinderMesh(start, end, rad1, rad2,
						seg == 0 ? -glm::normalize(s0) : glm::normalize(posneg1 - pos2), seg == segMins -1 ? -glm::normalize(s1) : glm::normalize(pos1 - pos3), points, faceCounts, faceConns, (currNode.RefChildHandles().size() == 0), false);
				}			

				prevFlow = currFlow;
				currFlowHand = currFlow.GetParentHandle();
				currFlow = nextFlow;
				nextFlow = currFlow.GetParentHandle() > -1 ? skeleton.PeekFlow(currFlow.GetParentHandle()) : nextFlow;
			} while (currFlow.GetParentHandle() != -1 || currFlowHand > -1);
			// will we still need to draw the very last one??
		}
	}
	// loop down by getting the "next" (or prev...)
	// construct bezier curve between
			// b0 = this strand
			// b1 = b0 + 1/3 S0. -> S0 = (strand +1 (b3) - strand -1) / 2. If no - then do + and curr.
			// b2 = b3 - 1/3 S1 -> S1 = (strand +2 - strand (b0)) / 2. If no +, then lock into horizontal-> 0, 1, 0.
			// b3 = next strand.
			// To get next flow particle strand: next, get node, check if this node "IsFlowStartNode()", until it is.
	// draw cylinder depending on "fineness" input int -> 1 is 1 cylinder per flow, with no interp




	/*for (int i = 0; i < skeleton.RefSortedFlowList().size(); ++i)
	{
		int currHandle = skeleton.RefSortedFlowList()[i];
		auto& curr = skeleton.PeekFlow(currHandle);
		glm::vec3 currPos = curr.m_info.m_globalStartPosition;
		glm::vec3 parentPos = curr.m_info.m_globalEndPosition;
		MPoint start(currPos[0], currPos[1], currPos[2]);
		MPoint end(parentPos[0], parentPos[1], parentPos[2]);
		glm::vec3 sDir;
		if (curr.GetParentHandle() >= 0) {
			sDir = currPos - skeleton.PeekFlow(curr.GetParentHandle()).m_info.m_globalStartPosition;
		}
		else {
			sDir = glm::vec3(0, 1, 0);
		}
			glm::vec3 eDir = parentPos - currPos;
		buildCylinderMesh(start, end, curr.m_info.m_startThickness * radius, curr.m_info.m_endThickness * radius, sDir, eDir,
			points, faceCounts, faceConns, node.RefChildHandles().size() == 0, false);*/
		//CylinderMesh cyl(start, end);
		//cyl.appendToMesh(points, faceCounts, faceConns, curr.m_info.m_startThickness * radius, curr.m_info.m_endThickness * radius, curr.m_info.m_globalStartRotation, curr.m_info.m_globalEndRotation);

#else
	for(auto &nodeHandle: skeleton.RefSortedNodeList()) {
		auto& node = skeleton.RefNode(nodeHandle);
		if (node.GetParentHandle() < 0) continue;

		auto& parentNode = skeleton.RefNode(node.GetParentHandle());
		std::unordered_map<NodeHandle, glm::vec3> parentGlobalPositions = strandManager.getGlobalNodeParticlePositions(parentNode.GetHandle(), skeleton);
		std::unordered_map<NodeHandle, glm::vec3> childGlobalPositions = strandManager.getGlobalNodeParticlePositions(nodeHandle, skeleton);

		for (auto& particle : childGlobalPositions) {
			glm::vec3 endPosition = particle.second;
			glm::vec3 startPosition = parentGlobalPositions[particle.first];

			glm::vec3 sDir;
			sDir = glm::vec3(0, 1, 0);

			MPoint start(startPosition[0], startPosition[1], startPosition[2]);
			MPoint end(endPosition[0], endPosition[1], endPosition[2]);

		//	glm::vec3 eDir = parentPos - currPos;
			buildCylinderMesh(start, end, parentNode.m_info.m_thickness * radius, node.m_info.m_thickness * radius, 
				endPosition - startPosition, endPosition - startPosition, points, faceCounts, faceConns, (node.RefChildHandles().size() == 0), false);
		}	
	}

	/*
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
		glm::vec3 sDir;
		if (parent.GetParentHandle() >= 0) {
			sDir = parentPos - skeleton.PeekNode(parent.GetParentHandle()).m_info.m_globalPosition;
		}
		else {
			sDir = glm::vec3(0, 1, 0);
		}
		glm::vec3 eDir = currPos - parentPos;
		buildCylinderMesh(start, end, parent.m_info.m_thickness * radius, curr.m_info.m_thickness * radius, sDir, eDir,
			points, faceCounts, faceConns);
			*/
#endif
	return true;
}

void TreeNode::developSubtree(ShootSkeleton& m_shootSkeleton, const NodeHandle& first, glm::vec3& lightDir) {
	// To develop the subtree, we will call everything in the regular grow function, but only
	// on this first node and its children

	// STEP 1: Collecting Shoot Flux -> light energy on leafs
	// first we need to get all the nodes of this subtree using DFS
	std::stack<NodeHandle> nhstack;
	std::unordered_set<NodeHandle> visited;
	nhstack.push(first);
	while (!nhstack.empty()) {
		const NodeHandle currNH = nhstack.top();
		nhstack.pop();
		if (!visited.count(currNH)) {
			visited.insert(currNH);
			std::vector<NodeHandle> children = m_shootSkeleton.RefNode(currNH).RefChildHandles();
			for (const auto& child : children) {
				nhstack.push(child);
			}
		}
	}
	m_shootSkeleton.m_data.m_treeIlluminationEstimator.m_voxel.Initialize(m_shootSkeleton.m_data.m_treeIlluminationEstimator.m_settings.m_voxelSize, m_shootSkeleton.m_min, m_shootSkeleton.m_max);

	const float maxLeafSize = glm::pow((treeParams.sgc.m_maxLeafSize.x + treeParams.sgc.m_maxLeafSize.z) / 2.0f, 2.0f);
	const float maxFruitSize = glm::pow((treeParams.sgc.m_maxFruitSize.x + treeParams.sgc.m_maxFruitSize.y + treeParams.sgc.m_maxFruitSize.z) / 3.0f, 2.0f);
	for (const auto& nodeHandle : visited) {
		const auto& internode = m_shootSkeleton.RefNode(nodeHandle);
		const auto& internodeData = internode.m_data;
		const auto& internodeInfo = internode.m_info;
		float shadowSize = internodeInfo.m_length * internodeInfo.m_thickness * 2.0f;
		for (const auto& i : internodeData.m_buds)
		{
			if (i.m_type == BudType::Leaf && i.m_reproductiveModule.m_maturity > 0.0f)
			{
				shadowSize += maxLeafSize * glm::pow(i.m_reproductiveModule.m_maturity, 0.5f);
			}
			else if (i.m_type == BudType::Fruit && i.m_reproductiveModule.m_maturity > 0.0f)
			{
				shadowSize += maxFruitSize * glm::pow(i.m_reproductiveModule.m_maturity, 1.0f / 3.0f);
			}
		}
		m_shootSkeleton.m_data.m_treeIlluminationEstimator.AddShadowVolume({ internodeInfo.m_globalPosition, shadowSize });
	}

	for (const auto& internodeHandle : visited) {
		auto& internode = m_shootSkeleton.RefNode(internodeHandle);
		auto& internodeData = internode.m_data;
		auto& internodeInfo = internode.m_info;
		// if this light dir hasn't been set yet, set it to our custom one
		if (internodeData.m_lightDirection == glm::vec3(0.f)) {
			internodeData.m_lightDirection = lightDir;
		}
   		internodeData.m_lightIntensity =
			m_shootSkeleton.m_data.m_treeIlluminationEstimator.IlluminationEstimation(internodeInfo.m_globalPosition, internodeData.m_lightDirection);
		for (const auto& bud : internode.m_data.m_buds)
		{
			if (bud.m_status == BudStatus::Flushed && bud.m_type == BudType::Leaf)
			{
				if (treeModel.m_treeGrowthSettings.m_collectLight) {
					internodeData.m_lightEnergy = internodeData.m_lightIntensity * glm::pow(bud.m_reproductiveModule.m_maturity, 2.0f) * bud.m_reproductiveModule.m_health;
					m_shootSkeleton.m_data.m_shootFlux.m_lightEnergy += internodeData.m_lightEnergy;
				}
			}
		}
	}

	// STEP 2: Vigor Allocation - photosynthesis
	RootSkeleton& m_rootSkeleton = treeModel.RefRootSkeleton();
	float totalVigor = m_shootSkeleton.m_data.m_shootFlux.m_lightEnergy;

	const float totalLeafMaintenanceVigorRequirement = m_shootSkeleton.m_data.m_vigorRequirement.m_leafMaintenanceVigor + m_rootSkeleton.m_data.m_vigorRequirement.m_leafMaintenanceVigor;
	const float totalLeafDevelopmentVigorRequirement = m_shootSkeleton.m_data.m_vigorRequirement.m_leafDevelopmentalVigor + m_rootSkeleton.m_data.m_vigorRequirement.m_leafDevelopmentalVigor;
	const float totalFruitMaintenanceVigorRequirement = m_shootSkeleton.m_data.m_vigorRequirement.m_fruitMaintenanceVigor + m_rootSkeleton.m_data.m_vigorRequirement.m_fruitMaintenanceVigor;
	const float totalFruitDevelopmentVigorRequirement = m_shootSkeleton.m_data.m_vigorRequirement.m_fruitDevelopmentalVigor + m_rootSkeleton.m_data.m_vigorRequirement.m_fruitDevelopmentalVigor;
	const float totalNodeDevelopmentalVigorRequirement = m_shootSkeleton.m_data.m_vigorRequirement.m_nodeDevelopmentalVigor + m_rootSkeleton.m_data.m_vigorRequirement.m_nodeDevelopmentalVigor;

	const float leafMaintenanceVigor = glm::min(totalVigor, totalLeafMaintenanceVigorRequirement);
	const float leafDevelopmentVigor = glm::min(totalVigor - totalLeafMaintenanceVigorRequirement, totalLeafDevelopmentVigorRequirement);
	const float fruitMaintenanceVigor = glm::min(totalVigor - totalLeafMaintenanceVigorRequirement - leafDevelopmentVigor, totalFruitMaintenanceVigorRequirement);
	const float fruitDevelopmentVigor = glm::min(totalVigor - totalLeafMaintenanceVigorRequirement - leafDevelopmentVigor - fruitMaintenanceVigor, totalFruitDevelopmentVigorRequirement);
	const float nodeDevelopmentVigor = glm::min(totalVigor - totalLeafMaintenanceVigorRequirement - leafDevelopmentVigor - fruitMaintenanceVigor - fruitDevelopmentVigor, totalNodeDevelopmentalVigorRequirement);
	m_rootSkeleton.m_data.m_vigor = m_shootSkeleton.m_data.m_vigor = 0.0f;
	if (totalLeafMaintenanceVigorRequirement != 0.0f) {
		m_rootSkeleton.m_data.m_vigor += leafMaintenanceVigor * m_rootSkeleton.m_data.m_vigorRequirement.m_leafMaintenanceVigor
			/ totalLeafMaintenanceVigorRequirement;
		m_shootSkeleton.m_data.m_vigor += leafMaintenanceVigor * m_shootSkeleton.m_data.m_vigorRequirement.m_leafMaintenanceVigor
			/ totalLeafMaintenanceVigorRequirement;
	}
	if (totalLeafDevelopmentVigorRequirement != 0.0f) {
		m_rootSkeleton.m_data.m_vigor += leafDevelopmentVigor * m_rootSkeleton.m_data.m_vigorRequirement.m_leafDevelopmentalVigor
			/ totalLeafDevelopmentVigorRequirement;
		m_shootSkeleton.m_data.m_vigor += leafDevelopmentVigor * m_shootSkeleton.m_data.m_vigorRequirement.m_leafDevelopmentalVigor
			/ totalLeafDevelopmentVigorRequirement;
	}
	if (totalFruitMaintenanceVigorRequirement != 0.0f) {
		m_rootSkeleton.m_data.m_vigor += fruitMaintenanceVigor * m_rootSkeleton.m_data.m_vigorRequirement.m_fruitMaintenanceVigor
			/ totalFruitMaintenanceVigorRequirement;
		m_shootSkeleton.m_data.m_vigor += fruitMaintenanceVigor * m_shootSkeleton.m_data.m_vigorRequirement.m_fruitMaintenanceVigor
			/ totalFruitMaintenanceVigorRequirement;
	}
	if (totalFruitDevelopmentVigorRequirement != 0.0f) {
		m_rootSkeleton.m_data.m_vigor += fruitDevelopmentVigor * m_rootSkeleton.m_data.m_vigorRequirement.m_fruitDevelopmentalVigor
			/ totalFruitDevelopmentVigorRequirement;
		m_shootSkeleton.m_data.m_vigor += fruitDevelopmentVigor * m_shootSkeleton.m_data.m_vigorRequirement.m_fruitDevelopmentalVigor
			/ totalFruitDevelopmentVigorRequirement;
	}

	treeModel.m_vigorRatio.m_rootVigorWeight = m_rootSkeleton.m_data.m_vigorRequirement.m_nodeDevelopmentalVigor / totalNodeDevelopmentalVigorRequirement;
	treeModel.m_vigorRatio.m_shootVigorWeight = m_shootSkeleton.m_data.m_vigorRequirement.m_nodeDevelopmentalVigor / totalNodeDevelopmentalVigorRequirement;

	if (treeModel.m_vigorRatio.m_shootVigorWeight + treeModel.m_vigorRatio.m_rootVigorWeight != 0.0f) {
		m_rootSkeleton.m_data.m_vigor += nodeDevelopmentVigor * treeModel.m_vigorRatio.m_rootVigorWeight / (treeModel.m_vigorRatio.m_shootVigorWeight + treeModel.m_vigorRatio.m_rootVigorWeight);
		m_shootSkeleton.m_data.m_vigor += nodeDevelopmentVigor * treeModel.m_vigorRatio.m_shootVigorWeight / (treeModel.m_vigorRatio.m_shootVigorWeight + treeModel.m_vigorRatio.m_rootVigorWeight);
	}

	// STEP 3: Grow branches and set up nutrient requirements for next iteration.
	bool treeStructureChanged = false;
	PlantGrowthRequirement newShootGrowthRequirement;
	glm::mat4 globalTransform = glm::mat4();
	if (growSubShoots(globalTransform, first, visited, treeParams.cm, treeParams.sgc, newShootGrowthRequirement)) {
		treeStructureChanged = true;
	}

	// STEP 4: Set tree age
	const int year = treeParams.cm.m_time;
	if (year != treeModel.m_ageInYear)
	{
		//const auto& sortedInternodeList = m_shootSkeleton.RefSortedNodeList();
		//for (auto it = sortedInternodeList.rbegin(); it != sortedInternodeList.rend(); it++) {
		for (auto nh : visited){
			auto& internode = m_shootSkeleton.RefNode(nh);
			auto& internodeData = internode.m_data;
			auto& buds = internodeData.m_buds;
			for (auto& bud : buds)
			{
				if (bud.m_status == BudStatus::Removed) continue;
				if (bud.m_type == BudType::Fruit || bud.m_type == BudType::Leaf)
				{
					bud.m_status = BudStatus::Dormant;
					bud.m_reproductiveModule.Reset();
				}
			}
		}
		treeModel.m_fruitCount = treeModel.m_leafCount = 0;


		treeModel.ResetReproductiveModule();
		treeModel.m_ageInYear = year;
	}
	//Set new growth nutrients requirement for next iteration.
	m_shootSkeleton.m_data.m_vigorRequirement = newShootGrowthRequirement;
	treeModel.m_iteration++;
	treeModel.m_age += treeModel.m_currentDeltaTime;
}

void TreeNode::pruneSubtree(ShootSkeleton& skeleton, const NodeHandle& first) {
	treeModel.PruneInternode(first);
	skeleton.SortLists();
}

bool TreeNode::growSubShoots(const glm::mat4& globalTransform, const NodeHandle& first, std::unordered_set<NodeHandle> subNodes,
	ClimateModel& climateModel, const ShootGrowthController& shootGrowthParameters, PlantGrowthRequirement& newShootGrowthRequirement) {
	ShootSkeleton& skeleton = treeModel.RefShootSkeleton();

	// Update the tree from this node down.
	// First, we need to PRUNE in case the vigor was decreased
	const auto maxDistance = skeleton.RefNode(0).m_data.m_maxDistanceToAnyBranchEnd;
	bool anyBranchPruned = false;
	for (const auto& internodeHandle : subNodes) {
		if (skeleton.RefNode(internodeHandle).IsRecycled()) continue;
		if (treeModel.PruneInternodes(maxDistance, internodeHandle, treeParams.sgc)) {
			anyBranchPruned = true;
			MGlobal::displayInfo("pruned");
		}
	}
	// if we pruned something, we have to resort flow and node handles, and thus
	// we need to recalculate the subtree nodes
	if (anyBranchPruned) {
		skeleton.SortLists();
		subNodes.clear();
		std::stack<NodeHandle> nhstack;
		nhstack.push(first);
		while (!nhstack.empty()) {
			const NodeHandle currNH = nhstack.top();
			nhstack.pop();
			if (!subNodes.count(currNH)) {
				subNodes.insert(currNH);
				std::vector<NodeHandle> children = skeleton.RefNode(currNH).RefChildHandles();
				for (const auto& child : children) {
					nhstack.push(child);
				}
			}
		}
	}

	// Now we GROW
	bool anyBranchGrown = false;
	// AggregateInternodeVigorRequirement
	for (auto nh : subNodes) {
		auto& internode = skeleton.RefNode(nh);
		auto& internodeData = internode.m_data;
		if (!internode.IsEndNode()) {
			//If current node is not end node
			for (const auto& i : internode.RefChildHandles()) {
				auto& childInternode = skeleton.RefNode(i);
				internodeData.m_vigorFlow.m_subtreeVigorRequirementWeight +=
					shootGrowthParameters.m_vigorRequirementAggregateLoss *
					(childInternode.m_data.m_vigorFlow.m_vigorRequirementWeight
						+ childInternode.m_data.m_vigorFlow.m_subtreeVigorRequirementWeight);
			}
		}
	}
	
	SubTreeAllocateShootVigor(first, subNodes, treeParams.sgc);

	// grow node
	for (const auto& internodeHandle : subNodes) {
		const bool graphChanged = treeModel.GrowInternode(treeParams.cm, internodeHandle, treeParams.sgc);
		anyBranchGrown = anyBranchGrown || graphChanged;
	}

	// again, resort if the skeleton changed and recalculate child nodes
	if (anyBranchGrown) {
		skeleton.SortLists();
		subNodes.clear();
		std::stack<NodeHandle> nhstack;
		nhstack.push(first);
		while (!nhstack.empty()) {
			const NodeHandle currNH = nhstack.top();
			nhstack.pop();
			if (!subNodes.count(currNH)) {
				subNodes.insert(currNH);
				std::vector<NodeHandle> children = skeleton.RefNode(currNH).RefChildHandles();
				for (const auto& child : children) {
					nhstack.push(child);
				}
			}
		}
	}
	growPostProcess(anyBranchGrown || anyBranchPruned, skeleton, subNodes);

	return anyBranchGrown || anyBranchPruned;
}

void TreeNode::SubTreeAllocateShootVigor(const NodeHandle& first, std::unordered_set<NodeHandle> subNodes, const ShootGrowthController& shootGrowthParameters) {
	// Commenting out all the parts that subtract vigor, as we don't want the growing of the subtree to shrivel the rest of the tree
	
	//Go from rooting point to all end nodes
	ShootSkeleton& m_shootSkeleton = treeModel.RefShootSkeleton();
	const float apicalControl = shootGrowthParameters.m_apicalControl;
	float remainingVigor = m_shootSkeleton.m_data.m_vigor;

	const float leafMaintenanceVigor = glm::min(remainingVigor, m_shootSkeleton.m_data.m_vigorRequirement.m_leafMaintenanceVigor);
	remainingVigor -= leafMaintenanceVigor;
	float leafMaintenanceVigorFillingRate = 0.0f;
	if (m_shootSkeleton.m_data.m_vigorRequirement.m_leafMaintenanceVigor != 0.0f)
		leafMaintenanceVigorFillingRate = leafMaintenanceVigor / m_shootSkeleton.m_data.m_vigorRequirement.m_leafMaintenanceVigor;

	const float leafDevelopmentVigor = glm::min(remainingVigor, m_shootSkeleton.m_data.m_vigorRequirement.m_leafDevelopmentalVigor);
	remainingVigor -= leafDevelopmentVigor;
	float leafDevelopmentVigorFillingRate = 0.0f;
	if (m_shootSkeleton.m_data.m_vigorRequirement.m_leafDevelopmentalVigor != 0.0f)
		leafDevelopmentVigorFillingRate = leafDevelopmentVigor / m_shootSkeleton.m_data.m_vigorRequirement.m_leafDevelopmentalVigor;

	const float fruitMaintenanceVigor = glm::min(remainingVigor, m_shootSkeleton.m_data.m_vigorRequirement.m_fruitMaintenanceVigor);
	remainingVigor -= fruitMaintenanceVigor;
	float fruitMaintenanceVigorFillingRate = 0.0f;
	if (m_shootSkeleton.m_data.m_vigorRequirement.m_fruitMaintenanceVigor != 0.0f)
		fruitMaintenanceVigorFillingRate = fruitMaintenanceVigor / m_shootSkeleton.m_data.m_vigorRequirement.m_fruitMaintenanceVigor;

	const float fruitDevelopmentVigor = glm::min(remainingVigor, m_shootSkeleton.m_data.m_vigorRequirement.m_fruitDevelopmentalVigor);
	remainingVigor -= fruitDevelopmentVigor;
	float fruitDevelopmentVigorFillingRate = 0.0f;
	if (m_shootSkeleton.m_data.m_vigorRequirement.m_fruitDevelopmentalVigor != 0.0f)
		fruitDevelopmentVigorFillingRate = fruitDevelopmentVigor / m_shootSkeleton.m_data.m_vigorRequirement.m_fruitDevelopmentalVigor;

	const float nodeDevelopmentVigor = glm::min(remainingVigor, m_shootSkeleton.m_data.m_vigorRequirement.m_nodeDevelopmentalVigor);
	if (m_shootSkeleton.m_data.m_vigorRequirement.m_nodeDevelopmentalVigor != 0.0f) {
		treeModel.m_internodeDevelopmentRate = nodeDevelopmentVigor / m_shootSkeleton.m_data.m_vigorRequirement.m_nodeDevelopmentalVigor;
	}

	for (const auto& internodeHandle : subNodes) {
		auto& internode = m_shootSkeleton.RefNode(internodeHandle);
		auto& internodeData = internode.m_data;
		auto& internodeVigorFlow = internodeData.m_vigorFlow;
		//1. Allocate maintenance vigor first
		for (auto& bud : internodeData.m_buds) {
			switch (bud.m_type)
			{
			case BudType::Leaf:
			{
				bud.m_vigorSink.AddVigor(leafMaintenanceVigorFillingRate * bud.m_vigorSink.GetMaintenanceVigorRequirement());
				bud.m_vigorSink.AddVigor(leafDevelopmentVigorFillingRate * bud.m_vigorSink.GetMaxVigorRequirement());
			}break;
			case BudType::Fruit:
			{
				bud.m_vigorSink.AddVigor(fruitMaintenanceVigorFillingRate * bud.m_vigorSink.GetMaintenanceVigorRequirement());
				bud.m_vigorSink.AddVigor(fruitDevelopmentVigorFillingRate * bud.m_vigorSink.GetMaxVigorRequirement());
			}break;
			default:break;
			}

		}
		//2. Allocate development vigor for structural growth
		//If this is the first node (node at the rooting point)
		if (internode.GetParentHandle() == -1) {
		//if (internodeHandle == first) {
			MGlobal::displayInfo("FIRST");
			internodeVigorFlow.m_allocatedVigor = 0.0f;
			internodeVigorFlow.m_subTreeAllocatedVigor = 0.0f;
			if (m_shootSkeleton.m_data.m_vigorRequirement.m_nodeDevelopmentalVigor != 0.0f) {
				/*for (const auto& bud : internodeData.m_buds) {
					if (bud.m_type == BudType::Apical) {
						internodeVigorFlow.m_vigorRequirementWeight += bud.m_vigorSink.GetDesiredDevelopmentalVigorRequirement();
					}
				}
				NodeHandle root = m_shootSkeleton.RefSortedNodeList()[0];
				const auto rootData = m_shootSkeleton.PeekNode(root).m_data;*/
				const float totalRequirement = internodeVigorFlow.m_vigorRequirementWeight + internodeVigorFlow.m_subtreeVigorRequirementWeight;
				if (totalRequirement != 0.0f) {
					//The root internode firstly extract it's own resources needed for itself.
					internodeVigorFlow.m_allocatedVigor = nodeDevelopmentVigor * internodeVigorFlow.m_vigorRequirementWeight / totalRequirement;
				}
				//internodeVigorFlow.m_allocatedVigor = nodeDevelopmentVigor * 0.5;
				//The rest resource will be distributed to the descendants. 
				internodeVigorFlow.m_subTreeAllocatedVigor = nodeDevelopmentVigor - internodeVigorFlow.m_allocatedVigor;
			}

		}
		//The buds will get its own resources
		for (auto& bud : internodeData.m_buds) {
			if (bud.m_type == BudType::Apical && internodeVigorFlow.m_vigorRequirementWeight != 0.0f) {
				//The vigor gets allocated and stored eventually into the buds

				// Added by Avi, because m_allocatedVigor keeps being 0
				int distToFirst = 0;
				auto currHand = internodeHandle;
				while (currHand != first) {
					currHand = m_shootSkeleton.PeekNode(currHand).GetParentHandle();
					++distToFirst;
				}
				float num = 1.0;
				num -= (distToFirst * 0.1);
				num = glm::clamp(num, 0.f, 1.5f);
				float vigorRatio = glm::clamp((float)(subNodes.size() - distToFirst * 0.5) / (float)m_shootSkeleton.RefSortedNodeList().size(), 0.f, 10.f);// (float)numParents / (float)subNodes.size() * 2.f;
				const float budAllocatedVigor = num * bud.m_vigorSink.GetMaxVigorRequirement() / internodeVigorFlow.m_vigorRequirementWeight;
				//const float budAllocatedVigor = internodeVigorFlow.m_allocatedVigor *
				//	bud.m_vigorSink.GetMaxVigorRequirement() / internodeVigorFlow.m_vigorRequirementWeight;
				bud.m_vigorSink.AddVigor(budAllocatedVigor);
			}
		}

		if (internodeVigorFlow.m_subTreeAllocatedVigor != 0.0f) {
			float childDevelopmentalVigorRequirementWeightSum = 0.0f;
			for (const auto& i : internode.RefChildHandles()) {
				const auto& childInternode = m_shootSkeleton.RefNode(i);
				const auto& childInternodeData = childInternode.m_data;
				auto& childInternodeVigorFlow = childInternodeData.m_vigorFlow;
				float childDevelopmentalVigorRequirementWeight = 0.0f;
				if (internodeVigorFlow.m_subtreeVigorRequirementWeight != 0.0f)childDevelopmentalVigorRequirementWeight = (childInternodeVigorFlow.m_vigorRequirementWeight + childInternodeVigorFlow.m_subtreeVigorRequirementWeight)
					/ internodeVigorFlow.m_subtreeVigorRequirementWeight;
				//Perform Apical control here.
				if (childInternodeData.m_isMaxChild) childDevelopmentalVigorRequirementWeight *= apicalControl;
				childDevelopmentalVigorRequirementWeightSum += childDevelopmentalVigorRequirementWeight;
			}
			for (const auto& i : internode.RefChildHandles()) {
				auto& childInternode = m_shootSkeleton.RefNode(i);
				auto& childInternodeData = childInternode.m_data;
				auto& childInternodeVigorFlow = childInternodeData.m_vigorFlow;
				float childDevelopmentalVigorRequirementWeight = 0.0f;
				if (internodeVigorFlow.m_subtreeVigorRequirementWeight != 0.0f)
					childDevelopmentalVigorRequirementWeight = (childInternodeVigorFlow.m_vigorRequirementWeight + childInternodeVigorFlow.m_subtreeVigorRequirementWeight)
					/ internodeVigorFlow.m_subtreeVigorRequirementWeight;

				//Re-perform apical control.
				if (childInternodeData.m_isMaxChild) childDevelopmentalVigorRequirementWeight *= apicalControl;

				//Calculate total amount of development vigor belongs to this child from internode received vigor for its children.
				float childTotalAllocatedDevelopmentVigor = 0.0f;
				if (childDevelopmentalVigorRequirementWeightSum != 0.0f) childTotalAllocatedDevelopmentVigor = internodeVigorFlow.m_subTreeAllocatedVigor *
					childDevelopmentalVigorRequirementWeight / childDevelopmentalVigorRequirementWeightSum;

				//Calculate allocated vigor.
				childInternodeVigorFlow.m_allocatedVigor = 0.0f;
				if (childInternodeVigorFlow.m_vigorRequirementWeight + childInternodeVigorFlow.m_subtreeVigorRequirementWeight != 0.0f) {
					childInternodeVigorFlow.m_allocatedVigor += childTotalAllocatedDevelopmentVigor *
						childInternodeVigorFlow.m_vigorRequirementWeight
						/ (childInternodeVigorFlow.m_vigorRequirementWeight + childInternodeVigorFlow.m_subtreeVigorRequirementWeight);
				}
				childInternodeVigorFlow.m_subTreeAllocatedVigor = childTotalAllocatedDevelopmentVigor - childInternodeVigorFlow.m_allocatedVigor;
			}
		}
		else
		{
			for (const auto& i : internode.RefChildHandles())
			{
				auto& childInternode = m_shootSkeleton.RefNode(i);
				auto& childInternodeData = childInternode.m_data;
				auto& childInternodeVigorFlow = childInternodeData.m_vigorFlow;
				childInternodeVigorFlow.m_allocatedVigor = childInternodeVigorFlow.m_subTreeAllocatedVigor = 0.0f;
			}
		}
	}
}

void TreeNode::growPostProcess(bool treeStructureChanged, ShootSkeleton& m_shootSkeleton, std::unordered_set<NodeHandle> subNodes) {
	PlantGrowthRequirement newShootGrowthRequirement;
	{
		m_shootSkeleton.m_min = glm::vec3(FLT_MAX);
		m_shootSkeleton.m_max = glm::vec3(FLT_MIN);
		for (auto nh : subNodes) {
			treeModel.CalculateThicknessAndSagging(nh, treeParams.sgc);
		}

		for (const auto& internodeHandle : subNodes) {
			auto& internode = m_shootSkeleton.RefNode(internodeHandle);
			auto& internodeData = internode.m_data;
			auto& internodeInfo = internode.m_info;

			if (internode.GetParentHandle() == -1) {
				internodeInfo.m_globalPosition = glm::vec3(0.0f);
				internodeInfo.m_localRotation = glm::vec3(0.0f);
				internodeInfo.m_globalRotation = internodeInfo.m_regulatedGlobalRotation = glm::vec3(glm::radians(90.0f), 0.0f, 0.0f);

				internodeData.m_rootDistance =
					internodeInfo.m_length / treeParams.sgc.m_internodeLength;
			}
			else {
				auto& parentInternode = m_shootSkeleton.RefNode(internode.GetParentHandle());
				internodeData.m_rootDistance = parentInternode.m_data.m_rootDistance + internodeInfo.m_length /
					treeParams.sgc.m_internodeLength;
				internodeInfo.m_globalRotation =
					parentInternode.m_info.m_globalRotation * internodeInfo.m_localRotation;
#pragma region Apply Sagging
				const auto& parentNode = m_shootSkeleton.RefNode(
					internode.GetParentHandle());
				auto parentGlobalRotation = parentNode.m_info.m_globalRotation;
				internodeInfo.m_globalRotation = parentGlobalRotation * internodeData.m_desiredLocalRotation;
				auto front = internodeInfo.m_globalRotation * glm::vec3(0, 0, -1);
				auto up = internodeInfo.m_globalRotation * glm::vec3(0, 1, 0);
				float dotP = glm::abs(glm::dot(front, treeModel.m_currentGravityDirection));
				TreeModel::ApplyTropism(treeModel.m_currentGravityDirection, internodeData.m_sagging * (1.0f - dotP), front, up);
				internodeInfo.m_globalRotation = glm::quatLookAt(front, up);
				internodeInfo.m_localRotation = glm::inverse(parentGlobalRotation) * internodeInfo.m_globalRotation;

				auto parentRegulatedUp = parentNode.m_info.m_regulatedGlobalRotation * glm::vec3(0, 1, 0);
				auto regulatedUp = glm::normalize(glm::cross(glm::cross(front, parentRegulatedUp), front));
				internodeInfo.m_regulatedGlobalRotation = glm::quatLookAt(front, regulatedUp);
#pragma endregion

				internodeInfo.m_globalPosition =
					parentInternode.m_info.m_globalPosition + parentInternode.m_info.m_length *
					(parentInternode.m_info.m_globalRotation *
						glm::vec3(0, 0, -1));


			}

			m_shootSkeleton.m_min = glm::min(m_shootSkeleton.m_min, internodeInfo.m_globalPosition);
			m_shootSkeleton.m_max = glm::max(m_shootSkeleton.m_max, internodeInfo.m_globalPosition);
			const auto endPosition = internodeInfo.m_globalPosition + internodeInfo.m_length *
				(internodeInfo.m_globalRotation *
					glm::vec3(0, 0, -1));
			m_shootSkeleton.m_min = glm::min(m_shootSkeleton.m_min, endPosition);
			m_shootSkeleton.m_max = glm::max(m_shootSkeleton.m_max, endPosition);
		}
		treeModel.SampleTemperature(glm::mat4(), treeParams.cm);
		treeModel.CalculateVigorRequirement(treeParams.sgc, newShootGrowthRequirement);

	};

	// uncomment this if want to mess with collision detection here
	/*if (m_treeGrowthSettings.m_enableBranchCollisionDetection)
	{
		const float minRadius = treeParams.sgc.m_endNodeThickness * 4.0f;
		CollisionDetection(minRadius, m_shootSkeleton.m_data.m_octree, m_shootSkeleton);
	}*/
	treeModel.m_internodeOrderCounts.clear();
	treeModel.m_fruitCount = treeModel.m_leafCount = 0;
	{
		int maxOrder = 0;
		const auto& sortedFlowList = m_shootSkeleton.RefSortedFlowList();
		for (const auto& flowHandle : sortedFlowList) {
			auto& flow = m_shootSkeleton.RefFlow(flowHandle);
			auto& flowData = flow.m_data;
			if (flow.GetParentHandle() == -1) {
				flowData.m_order = 0;
			}
			else {
				auto& parentFlow = m_shootSkeleton.RefFlow(flow.GetParentHandle());
				if (flow.IsApical()) flowData.m_order = parentFlow.m_data.m_order;
				else flowData.m_order = parentFlow.m_data.m_order + 1;
			}
			maxOrder = glm::max(maxOrder, flowData.m_order);
		}
		treeModel.m_internodeOrderCounts.resize(maxOrder + 1);
		std::fill(treeModel.m_internodeOrderCounts.begin(), treeModel.m_internodeOrderCounts.end(), 0);
		const auto& sortedInternodeList = m_shootSkeleton.RefSortedNodeList();
		for (const auto& internodeHandle : sortedInternodeList)
		{
			auto& internode = m_shootSkeleton.RefNode(internodeHandle);
			const auto order = m_shootSkeleton.RefFlow(internode.GetFlowHandle()).m_data.m_order;
			internode.m_data.m_order = order;
			treeModel.m_internodeOrderCounts[order]++;

			for (const auto& bud : internode.m_data.m_buds)
			{
				if (bud.m_status != BudStatus::Flushed || bud.m_reproductiveModule.m_maturity <= 0) continue;
				if (bud.m_type == BudType::Fruit)
				{
					treeModel.m_fruitCount++;
				}
				else if (bud.m_type == BudType::Leaf)
				{
					treeModel.m_leafCount++;
				}
			}

		}
		m_shootSkeleton.CalculateFlows();
	}
	m_shootSkeleton.m_data.m_vigorRequirement = newShootGrowthRequirement;
}

MString TreeNode::getLoadBar(int curr, int tot) {
	int pct = curr * 100 / tot;

	switch (pct / 10)
	{
	case(0):
		return "[----------] 0%";
	case(1):
		return "[#---------] 10%";
	case(2):
		return "[##--------] 20%";
	case(3):
		return "[###-------] 30%";
	case(4):
		return "[####------] 40%";
	case(5):
		return "[#####-----] 50%";
	case(6):
		return "[######----] 60%";
	case(7):
		return "[#######---] 70%";
	case(8):
		return "[########--] 80%";
	case(9):
		return "[#########-] 90%";
	default:
		break;
	}
}

// PARSING FILES

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
		else if (key == "m_fruitFallProbability:") {
			iss >> params.m_fruitFallProbability;
			//break;	// break on the last line
		}
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

void initializeParams(const std::string& treeName, PlantParameters& params) {
	if (treeName == "Birch") {
		MGlobal::displayInfo("birch");
		//SHOOT
		params.sg.m_internodeGrowthRate = 10.f;
		params.sg.m_leafGrowthRate = 18.f;
		params.sg.m_fruitGrowthRate = 18.f;
		params.sg.m_lateralBudCount = 2.f;
		params.sg.m_fruitBudCount = 1.f;
		params.sg.m_leafBudCount = 1.f;
		params.sg.m_branchingAngleMeanVariance[0] = 25.f;
		params.sg.m_branchingAngleMeanVariance[1] = 2.f;
		params.sg.m_rollAngleMeanVariance[0] = 120.f;
		params.sg.m_rollAngleMeanVariance[1] = 2.f;
		params.sg.m_apicalAngleMeanVariance[0] = 0.f;
		params.sg.m_apicalAngleMeanVariance[1] = 2;
		params.sg.m_gravitropism = 0.00999999978;
		params.sg.m_phototropism = 0.0399999991;
		params.sg.m_internodeLength = 0.0299999993;
		params.sg.m_endNodeThickness = 0.00200000009;
		params.sg.m_thicknessAccumulationFactor = 0.5;
		params.sg.m_thicknessAccumulateAgeFactor = 0.f;
		params.sg.m_lateralBudFlushingProbabilityTemperatureRange[0] = 0.00249999994;
		params.sg.m_lateralBudFlushingProbabilityTemperatureRange[1] = 0.00249999994;
		params.sg.m_lateralBudFlushingProbabilityTemperatureRange[2] = 0.f;
		params.sg.m_lateralBudFlushingProbabilityTemperatureRange[3] = 100.f;
		params.sg.m_leafBudFlushingProbabilityTemperatureRange[0] = 1.f;
		params.sg.m_leafBudFlushingProbabilityTemperatureRange[1] = 1.f;
		params.sg.m_leafBudFlushingProbabilityTemperatureRange[2] = 45.f;
		params.sg.m_leafBudFlushingProbabilityTemperatureRange[3] = 60.f;
		params.sg.m_fruitBudFlushingProbabilityTemperatureRange[0] = 0.f;
		params.sg.m_fruitBudFlushingProbabilityTemperatureRange[1] = 0.100000001;
		params.sg.m_fruitBudFlushingProbabilityTemperatureRange[2] = 50.f;
		params.sg.m_fruitBudFlushingProbabilityTemperatureRange[3] = 70.f;
		params.sg.m_apicalBudLightingFactor = 0.f;
		params.sg.m_lateralBudLightingFactor = 1.f;
		params.sg.m_leafBudLightingFactor = 1.f;
		params.sg.m_fruitBudLightingFactor = 1.f;
		params.sg.m_apicalControl = 0.f;
		params.sg.m_apicalControlAgeFactor = 0.f;
		params.sg.m_apicalDominance = 3.f;
		params.sg.m_apicalDominanceAgeFactor = 0.f;
		params.sg.m_apicalDominanceDistanceFactor = 0.970000029;
		params.sg.m_apicalBudExtinctionRate = 0.f;
		params.sg.m_lateralBudExtinctionRate = 0.f;
		params.sg.m_leafBudExtinctionRate = 0.f;
		params.sg.m_fruitBudExtinctionRate = 0.f;
		params.sg.m_leafVigorRequirement = 0.100000001;
		params.sg.m_fruitVigorRequirement = 0.100000001;
		params.sg.m_internodeVigorRequirement = 1.f;
		params.sg.m_vigorRequirementAggregateLoss = 0.989999988;
		params.sg.m_lowBranchPruning = 0.250000006;
		params.sg.m_saggingFactorThicknessReductionMax[0] = 9.99999975e-06;
		params.sg.m_saggingFactorThicknessReductionMax[1] = 2.f;
		params.sg.m_saggingFactorThicknessReductionMax[2] = 0.5;
		params.sg.m_maxLeafSize[0] = 0.0500000007;
		params.sg.m_maxLeafSize[1] = 0.5;
		params.sg.m_maxLeafSize[2] = 0.0500000007;
		params.sg.m_leafPositionVariance = 0.5;
		params.sg.m_leafRandomRotation = 10.f;
		params.sg.m_leafChlorophyllLoss = 4.f;
		params.sg.m_leafChlorophyllSynthesisFactorTemperature = 65.f;
		params.sg.m_leafFallProbability = 30.f;
		params.sg.m_leafDistanceToBranchEndLimit = 10.f;
		params.sg.m_maxFruitSize[0] = 0.0350000001;
		params.sg.m_maxFruitSize[1] = 0.0350000001;
		params.sg.m_maxFruitSize[2] = 0.0350000001;
		params.sg.m_fruitPositionVariance = 0.5;
		params.sg.m_fruitRandomRotation = 10.f;
		params.sg.m_fruitFallProbability = 3.f;
		//ROOT
		params.rg.m_branchingAngleMeanVariance[0] = 30.f;
		params.rg.m_branchingAngleMeanVariance[1] = 2.f;
		params.rg.m_rollAngleMeanVariance[0] = 120.f;
		params.rg.m_rollAngleMeanVariance[1] = 2.f;
		params.rg.m_apicalAngleMeanVariance[0] = 0.f;
		params.rg.m_apicalAngleMeanVariance[1] = 2.f;
		params.rg.m_rootNodeLength = 0.0299999993;
		params.rg.m_rootNodeGrowthRate = 10.f;
		params.rg.m_endNodeThickness = 0.00200000009;
		params.rg.m_thicknessAccumulationFactor = 0.5;
		params.rg.m_thicknessAccumulateAgeFactor = 0.f;
		params.rg.m_rootNodeVigorRequirement = 1.f;
		params.rg.m_vigorRequirementAggregateLoss = 1.f;
		params.rg.m_environmentalFriction = 1.f;
		params.rg.m_environmentalFrictionFactor = 0.25;
		params.rg.m_apicalControl = 0.f;
		params.rg.m_apicalControlAgeFactor = 0.f;
		params.rg.m_apicalDominance = 3.f;
		params.rg.m_apicalDominanceAgeFactor = 0.f;
		params.rg.m_apicalDominanceDistanceFactor = 0.970000029;
		params.rg.m_tropismSwitchingProbability = 1.f;
		params.rg.m_tropismSwitchingProbabilityDistanceFactor = 0.f;
		params.rg.m_tropismIntensity = 0.100000001;
		params.rg.m_branchingProbability = 0.0299999993;
	}
	else if (treeName == "Spruce") {
		MGlobal::displayInfo("Philly tree");
		//SHOOT
		params.sg.m_internodeGrowthRate = 8;
		params.sg.m_leafGrowthRate = 18;
		params.sg.m_fruitGrowthRate = 18;
		params.sg.m_lateralBudCount = 2;
		params.sg.m_fruitBudCount = 1;
		params.sg.m_leafBudCount = 1;
		params.sg.m_branchingAngleMeanVariance[0] = 60;
		params.sg.m_branchingAngleMeanVariance[1] = 3;
		params.sg.m_rollAngleMeanVariance[0] = 90;
		params.sg.m_rollAngleMeanVariance[1] = 2;
		params.sg.m_apicalAngleMeanVariance[0] = 0;
		params.sg.m_apicalAngleMeanVariance[1] = 1;
		params.sg.m_gravitropism = 0.0399999991;
		params.sg.m_phototropism = 0;
		params.sg.m_internodeLength = 0.0299999993;
		params.sg.m_endNodeThickness = 0.00100000005;
		params.sg.m_thicknessAccumulationFactor = 0.5;
		params.sg.m_thicknessAccumulateAgeFactor = 0;
		params.sg.m_lateralBudFlushingProbabilityTemperatureRange[0] = 0.0399999991;
		params.sg.m_lateralBudFlushingProbabilityTemperatureRange[1] = 0.0399999991;
		params.sg.m_lateralBudFlushingProbabilityTemperatureRange[2] = 0;
		params.sg.m_lateralBudFlushingProbabilityTemperatureRange[3] = 100;
		params.sg.m_leafBudFlushingProbabilityTemperatureRange[0] = 1;
		params.sg.m_leafBudFlushingProbabilityTemperatureRange[1] = 1;
		params.sg.m_leafBudFlushingProbabilityTemperatureRange[2] = 45;
		params.sg.m_leafBudFlushingProbabilityTemperatureRange[3] = 60;
		params.sg.m_fruitBudFlushingProbabilityTemperatureRange[0] = 0;
		params.sg.m_fruitBudFlushingProbabilityTemperatureRange[1] = 1;
		params.sg.m_fruitBudFlushingProbabilityTemperatureRange[2] = 50;
		params.sg.m_fruitBudFlushingProbabilityTemperatureRange[3] = 70;
		params.sg.m_apicalBudLightingFactor = 0;
		params.sg.m_lateralBudLightingFactor = 1;
		params.sg.m_leafBudLightingFactor = 1;
		params.sg.m_fruitBudLightingFactor = 1;
		params.sg.m_apicalControl = 0.5;
		params.sg.m_apicalControlAgeFactor = 0.100000001;
		params.sg.m_apicalDominance = 0;
		params.sg.m_apicalDominanceAgeFactor = 0;
		params.sg.m_apicalDominanceDistanceFactor = 0.899999976;
		params.sg.m_apicalBudExtinctionRate = 0;
		params.sg.m_lateralBudExtinctionRate = 0;
		params.sg.m_leafBudExtinctionRate = 0;
		params.sg.m_fruitBudExtinctionRate = 0;
		params.sg.m_leafVigorRequirement = 0.100000001;
		params.sg.m_fruitVigorRequirement = 0.100000001;
		params.sg.m_internodeVigorRequirement = 1;
		params.sg.m_vigorRequirementAggregateLoss = 0.949999988;
		params.sg.m_lowBranchPruning = 0.100000001;
		params.sg.m_saggingFactorThicknessReductionMax[0] = 0.00000999999978;
		params.sg.m_saggingFactorThicknessReductionMax[1] = 2;
		params.sg.m_saggingFactorThicknessReductionMax[2] = 0.200000003;
		params.sg.m_maxLeafSize[0] = 0.0500000007;
		params.sg.m_maxLeafSize[1] = 0.5;
		params.sg.m_maxLeafSize[2] = 0.0500000007;
		params.sg.m_leafPositionVariance = 0.5;
		params.sg.m_leafRandomRotation = 10;
		params.sg.m_leafChlorophyllLoss = 4;
		params.sg.m_leafChlorophyllSynthesisFactorTemperature = 65;
		params.sg.m_leafFallProbability = 3;
		params.sg.m_leafDistanceToBranchEndLimit = 10;
		params.sg.m_maxFruitSize[0] = 0.0350000001;
		params.sg.m_maxFruitSize[1] = 0.0350000001;
		params.sg.m_maxFruitSize[2] = 0.0350000001;
		params.sg.m_fruitPositionVariance = 0.5;
		params.sg.m_fruitRandomRotation = 10;
		params.sg.m_fruitFallProbability = 3;
		//ROOT
		params.rg.m_branchingAngleMeanVariance[0] = 30;
		params.rg.m_branchingAngleMeanVariance[1] = 2;
		params.rg.m_rollAngleMeanVariance[0] = 120;
		params.rg.m_rollAngleMeanVariance[1] = 2;
		params.rg.m_apicalAngleMeanVariance[0] = 0;
		params.rg.m_apicalAngleMeanVariance[1] = 2;
		params.rg.m_rootNodeLength = 0.0299999993;
		params.rg.m_rootNodeGrowthRate = 10;
		params.rg.m_endNodeThickness = 0.00200000009;
		params.rg.m_thicknessAccumulationFactor = 0.5;
		params.rg.m_thicknessAccumulateAgeFactor = 0;
		params.rg.m_rootNodeVigorRequirement = 1;
		params.rg.m_vigorRequirementAggregateLoss = 1;
		params.rg.m_environmentalFriction = 1;
		params.rg.m_environmentalFrictionFactor = 6;
		params.rg.m_apicalControl = 0;
		params.rg.m_apicalControlAgeFactor = 0;
		params.rg.m_apicalDominance = 2;
		params.rg.m_apicalDominanceAgeFactor = 0;
		params.rg.m_apicalDominanceDistanceFactor = 0.970000029;
		params.rg.m_tropismSwitchingProbability = 1;
		params.rg.m_tropismSwitchingProbabilityDistanceFactor = 0;
		params.rg.m_tropismIntensity = 0.150000006;
		params.rg.m_branchingProbability = 0.0299999993;
	}
	else {
		MGlobal::displayError("Preset tree name not recognized");
	}
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

bool TreeNode::ReadTreeParams(const std::string& treeName, RootGrowthController& m_rootGrowthParameters, ShootGrowthController& m_shootGrowthParameters,
	ClimateModel& cm, bool fromFile) {
	PlantParameters params;
	if (fromFile) {
		if (!parseConfigFile(treeName, params)) {
			return false;
		}
	}
	else if (!fromFile) {
		initializeParams(treeName, params);
	}

	size_t f = sizeof(params);

	std::cout << f << std::endl;

	MGlobal::displayInfo("Reading Tree File");
	// ROOT
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

void TreeNode::InitializeMVars(const std::string& treeName, SoilModel& m_soilModel, ClimateModel& m_climateModel,
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

	bool fromFile = treeName != "Birch" && treeName != "Spruce";

	// if we don't define a file path or couldn't read the file, we will initialize it as a birch
	if (treeName == "" || !ReadTreeParams(treeName, m_rootGrowthParameters, m_shootGrowthParameters, m_climateModel, fromFile)) {
		MGlobal::displayInfo("default Birch vals");
		ReadTreeParams("Birch", m_rootGrowthParameters, m_shootGrowthParameters, m_climateModel, false);
	}

	MGlobal::displayInfo("Tree Initialized");
}