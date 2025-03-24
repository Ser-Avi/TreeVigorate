#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include "TreeCommand.h"
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

    // Registering TreeCmd
    status = plugin.registerCommand("TreeCmd", TreeCmd::creator, TreeCmd::newSyntax);

    MGlobal::displayInfo("Registered Cmd");

	std::string windowCommand = R"(
global proc browseTreeDataFile() {
    string $filePath[] = `fileDialog2 -fileMode 1 -caption "Select Tree Data JSON"`;
    if (size($filePath) > 0) {
        textField -e -text ($filePath[0]) treeDataField;
    }
}

global proc browseTreeSpeciesFile() {
    string $filePath[] = `fileDialog2 -fileMode 1 -caption "Select Tree Species File"`;
    if (size($filePath) > 0) {
        textField -e -text ($filePath[0]) treeSpeciesField;
    }
}

global proc createTreeNode() {
	createNode transform -n TSys1;
	createNode mesh -n TShape1 -p TSys1;
	sets -add initialShadingGroup TShape1;
	createNode TreeNode -n TN1;
	connectAttr TN1.outputMesh TShape1.inMesh;
};

global proc generateTree() {
    //string $treeData = `textField -q -text treeDataField`;
    string $treeSpecies = `textField -q -text treeSpeciesField`;

    //if ($treeData == "" || $treeSpecies == "") {
    //    warning "Please select both a tree data JSON and a tree species file.";
    //    return;
    //}

    //print ("Generating tree with:\nTree Data: " + $treeData + "\nTree Species: " + $treeSpecies + "\n");
    print("Generating tree node");
    createTreeNode;
    
    // Replace the print statement with the actual tree generation logic
}

// Create the UI Window
global proc createTreeGeneratorWindow() {
    if (`window -exists generateTreeWindow`) {
        deleteUI generateTreeWindow;
    }

    window -title "Generate Tree" -widthHeight 450 200 generateTreeWindow;
    columnLayout -adjustableColumn true;

    // Add spacing at the top
    separator -style "none" -height 10;

    // Tree Data JSON Selection
    text -label "Select Tree Data JSON:" -align "left";
    separator -style "none" -height 5;
    rowLayout -numberOfColumns 2 -columnWidth2 350 80;
    textField -editable false -width 350 treeDataField;
    button -label "Browse" -command("browseTreeDataFile");
    setParent ..;

    // Add some space
    separator - style "none" - height 10;
    separator - style "in" - height 5;
    separator - style "none" - height 10;

    // Tree Species File Selection
    text - label "Select Tree Species File:" - align "left";
    separator - style "none" - height 5;
    rowLayout - numberOfColumns 2 - columnWidth2 350 80;
    textField - editable false - width 350 treeSpeciesField;
    button - label "Browse" - command("browseTreeSpeciesFile");
    setParent ..;

    // Add some space before the button
    separator - style "none" - height 15;
    separator - style "in" - height 5;
    separator - style "none" - height 15;

    // Generate Tree Button
    button - label "Generate Tree" - command("generateTree") - height 50;

    // Add spacing at the bottom
    separator - style "none" - height 10;

    showWindow generateTreeWindow;
}
)";

    std::string menuCommand = R"(
if (`menu -exists treeVigorateMenu`) {
    deleteUI treeVigorateMenu;
}

// Create the custom menu in the "Window" menu
menu
    -label "TreeVigorate"
    -parent MayaWindow
    treeVigorateMenu;

// Add the "Open Grammar Settings" item to the custom menu
menuItem
    -label "Build Tree"
    -command("createTreeGeneratorWindow")
        systemItem;
)";

    MGlobal::executeCommand(windowCommand.c_str());
    MGlobal::executeCommand(menuCommand.c_str());


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