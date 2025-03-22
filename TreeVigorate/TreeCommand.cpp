#include "TreeCommand.h"


#include <maya/MGlobal.h>
#include <maya/MArgList.h>
#include <list>
using namespace EcoSysLab;


TreeCmd::TreeCmd() : MPxCommand()
{
}

TreeCmd::~TreeCmd()
{
}

MSyntax TreeCmd::newSyntax() {
	MSyntax syntax;
	//params needed:
	//global transform

	return syntax;
}

MStatus TreeCmd::doIt(const MArgList& args)
{
	TreeModel treeModel = TreeModel();
	//bool Grow(float deltaTime, const glm::mat4 & globalTransform, SoilModel & soilModel, ClimateModel & climateModel,
	//const RootGrowthController & rootGrowthParameters, const ShootGrowthController & shootGrowthParameters);
	treeModel.Grow(.1f, glm::mat4(), m_soilModel, m_climateModel, m_rootGrowthParameters, m_shootGrowthParameters);

	return MStatus::kSuccess;
}

