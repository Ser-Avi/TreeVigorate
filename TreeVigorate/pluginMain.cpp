#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include "TreeNode.h"

#include <maya/MPxCommand.h>
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MArgList.h>
#include <maya/MSimple.h>
#include <maya/MDoubleArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MDGModifier.h>
#include <maya/MPlugArray.h>
#include <maya/MVector.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MStringArray.h>

MStatus initializePlugin(MObject obj) {
	const char* pluginVendor = "Avi and Eli :)";
	const char* pluginVersion = "0.2";

	MStatus   status = MStatus::kSuccess;
	MFnPlugin plugin(obj, pluginVendor, pluginVersion);

	// adding TreeNode
	status = plugin.registerNode("TreeNode", TreeNode::id,
		TreeNode::creator, TreeNode::initialize);
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	MGlobal::displayInfo("Plugin initialized successfully!");

	return (MS::kSuccess);
}

MStatus uninitializePlugin(MObject obj) {
	MStatus   status = MStatus::kSuccess;
	MFnPlugin plugin(obj);

	status = plugin.deregisterNode(TreeNode::id);
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	MGlobal::displayInfo("Plugin unitialized successfully!");

	return(MS::kSuccess);
}