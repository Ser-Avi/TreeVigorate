#pragma once

#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <string>
#include <RhizoCode/TreeModel.hpp>
#include <RhizoCode/ClimateModel.hpp>
#include <RhizoCode/SoilModel.hpp>
#include <RhizoCode/TreeGrowthController.hpp>

using namespace EcoSysLab;

class TreeCmd : public MPxCommand
{
public:
    TreeCmd();
    virtual ~TreeCmd();
    static void* creator() { return new TreeCmd(); }
    static MSyntax newSyntax();
    MStatus doIt(const MArgList& args);

    SoilModel m_soilModel;
    ClimateModel m_climateModel;
    RootGrowthController m_rootGrowthParameters;
    ShootGrowthController m_shootGrowthParameters;
};