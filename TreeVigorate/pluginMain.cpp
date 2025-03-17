#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>

MStatus initializePlugin(MObject obj) {
	const char* pluginVendor = "Avi and Eli";
	const char* pluginVersion = "0.1";

	MFnPlugin fnPlugin(obj, pluginVendor, pluginVersion);

	MGlobal::displayInfo("Plugin initialized successfully!");

	return (MS::kSuccess);
}

MStatus uninitializePlugin(MObject obj) {
	MGlobal::displayInfo("Plugin unitialized successfully!");

	return(MS::kSuccess);
}