#include "TreeCommand.h"


#include <maya/MGlobal.h>
#include <maya/MArgList.h>
#include <list>
#include <glm/gtc/random.hpp>
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

void SetSoilLayer(SoilLayer& sl) {

	sl.m_mat = SoilPhysicalMaterial({ 1,
		[](const glm::vec3& pos) { return 1.0f; },
		[](const glm::vec3& pos) { return 0.0f; },
		[](const glm::vec3& pos) { return 0.0f; },
		[](const glm::vec3& pos) { return 0.0f; },
		[](const glm::vec3& pos) { return 0.0f; } });
	sl.m_mat.m_soilMaterialTexture = std::make_shared<SoilMaterialTexture>();
	sl.m_mat.m_soilMaterialTexture->m_color_map.resize(2*2);
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

MStatus TreeCmd::doIt(const MArgList& args)
{

	TreeModel treeModel = TreeModel();
	int roots = treeModel.RefShootSkeleton().RefSortedNodeList().size();
	MGlobal::displayInfo("Initial Root num: ");
	MGlobal::displayInfo(MString(std::to_string(roots).c_str()));
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

	// ROOT - elm
	m_rootGrowthParameters.m_apicalAngle = [=](const Node<RootNodeGrowthData>& rootNode)
		{
			// This is from apicalAngleMeanVariance
			return glm::gaussRand(0, 3);
		};
	m_rootGrowthParameters.m_apicalControl = 0;
	m_rootGrowthParameters.m_apicalDominance = [=](const Node<RootNodeGrowthData>& rootNode)
		{
			// apicalDominance * glm::exp(-apicalDominanceAgeFactor, m_age)
			return 0;	//apical dominance age factor is 0 for elm -> whole thing is 0
		};
	m_rootGrowthParameters.m_apicalDominanceDistanceFactor = 0.970000029;
	m_rootGrowthParameters.m_branchingAngle = [=](const Node<RootNodeGrowthData>& rootNode)
		{
			// breanchingAngleMeanVariance
			return glm::gaussRand(30, 2);
		};
	m_rootGrowthParameters.m_branchingProbability = [=](const Node<RootNodeGrowthData>& rootNode)
		{
			return  0.0700000003;
		};
	m_rootGrowthParameters.m_endNodeThickness = 0.00200000009;
	m_rootGrowthParameters.m_environmentalFriction = [=](const Node<RootNodeGrowthData>& rootNode)
		{
			const auto& rootNodeData = rootNode.m_data;
			// glm::pow(1.0 / glm::max(rootNodeData.m_soilDensity * m_environmentalFritcion, 1.0), m_environmentalFrictionFactor)
			return 1.0f - glm::pow(1.0f / glm::max(rootNodeData.m_soilDensity * 0.5, 1.0), 1.0);
		};
	m_rootGrowthParameters.m_fineRootApicalAngleVariance = 0.3;
	m_rootGrowthParameters.m_fineRootBranchingAngle = 30;
	m_rootGrowthParameters.m_fineRootMinNodeThickness = 0.1;
	m_rootGrowthParameters.m_fineRootNodeCount = 2;
	m_rootGrowthParameters.m_fineRootSegmentLength = 0.2;
	m_rootGrowthParameters.m_fineRootThickness = 0.1;
	m_rootGrowthParameters.m_rollAngle = [=](const Node<RootNodeGrowthData>& rootNode)
		{
			//m_rollAngleMeanVariance
			return glm::gaussRand(120, 2);
		};
	m_rootGrowthParameters.m_rootNodeGrowthRate = 10;
	m_rootGrowthParameters.m_rootNodeLength = 0.0299999993;
	m_rootGrowthParameters.m_rootNodeVigorRequirement = 1;
	m_rootGrowthParameters.m_thicknessAccumulateAgeFactor = 0;
	m_rootGrowthParameters.m_thicknessAccumulationFactor = 0.5;
	m_rootGrowthParameters.m_tropismIntensity = 0.0500000007;
	m_rootGrowthParameters.m_tropismSwitchingProbability = 1;
	m_rootGrowthParameters.m_tropismSwitchingProbabilityDistanceFactor = 0;
	m_rootGrowthParameters.m_vigorRequirementAggregateLoss = 1;

	// CLIMATE
	ClimateParameters climateParametrs;
	m_climateModel.Initialize(climateParametrs);

	// SHOOT - Elm
	m_shootGrowthParameters.m_internodeGrowthRate = 10;
	m_shootGrowthParameters.m_leafGrowthRate = 18;
	m_shootGrowthParameters.m_fruitGrowthRate = 18;
	m_shootGrowthParameters.m_lateralBudCount = 2;
	m_shootGrowthParameters.m_fruitBudCount = 0;
	m_shootGrowthParameters.m_leafBudCount = 1;
	m_shootGrowthParameters.m_branchingAngle = [=](const Node<InternodeGrowthData>& internode)
		{
			return glm::gaussRand(60, 3);
		};
	m_shootGrowthParameters.m_rollAngle = [=](const Node<InternodeGrowthData>& internode)
		{
			return glm::gaussRand(90, 2);
		};
	m_shootGrowthParameters.m_apicalAngle = [=](const Node<InternodeGrowthData>& internode)
		{
			return glm::gaussRand(0, 2);
		};
	m_shootGrowthParameters.m_gravitropism = [=](const Node<InternodeGrowthData>& internode)
		{
			return 0.0299999993;
		};
	m_shootGrowthParameters.m_phototropism = [=](const Node<InternodeGrowthData>& internode)
		{
			return 0.0500000007;
		};
	m_shootGrowthParameters.m_internodeLength = 0.0299999993;
	m_shootGrowthParameters.m_endNodeThickness = 0.00200000009;
	m_shootGrowthParameters.m_thicknessAccumulationFactor = 0.5;
	m_shootGrowthParameters.m_thicknessAccumulateAgeFactor = 0;

	m_shootGrowthParameters.m_lateralBudFlushingProbability = [=](const Node<InternodeGrowthData>& internode)
		{
			glm::vec4 probabilityRange = glm::vec4(0.00499999989, 0.00499999989, 0, 100);
			const auto& internodeData = internode.m_data;
			float flushProbability = glm::mix(probabilityRange.x, probabilityRange.y,
				glm::clamp((internodeData.m_temperature - probabilityRange.z) / (probabilityRange.w - probabilityRange.z), 0.0f, 1.0f));
			if (internodeData.m_inhibitor > 0.0f) flushProbability *= glm::exp(-internodeData.m_inhibitor);
			//1 is shootGrowthParameters.m_lateralBudLightingFactor:
			flushProbability *= glm::pow(internodeData.m_lightIntensity, 1);
			return flushProbability;
		};
	m_shootGrowthParameters.m_leafBudFlushingProbability = [=](const Node<InternodeGrowthData>& internode)
		{
			const auto& internodeData = internode.m_data;
			const auto& probabilityRange = glm::vec4(1, 1, 45, 60);
			float flushProbability = glm::mix(probabilityRange.x, probabilityRange.y,
				glm::clamp((internodeData.m_temperature - probabilityRange.z) / (probabilityRange.w - probabilityRange.z), 0.0f, 1.0f));
			//1 is shootGrowthParameters.m_leafBudLightingFactor:
			flushProbability *= glm::pow(internodeData.m_lightIntensity, 1);
			return flushProbability;
		};
	m_shootGrowthParameters.m_fruitBudFlushingProbability = [=](const Node<InternodeGrowthData>& internode)
		{
			const auto& internodeData = internode.m_data;
			const auto& probabilityRange = glm::vec4(0, 1, 50, 70);
			float flushProbability = glm::mix(probabilityRange.x, probabilityRange.y,
				glm::clamp((internodeData.m_temperature - probabilityRange.z) / (probabilityRange.w - probabilityRange.z), 0.0f, 1.0f));
			//1 is shootGrowthParameters.m_fruitBudLightingFactor:
			flushProbability *= glm::pow(internodeData.m_lightIntensity, 1);
			return flushProbability;
		};

	//m_shootGrowthParameters.m_apicalBudLightingFactor = 0															;
	//m_shootGrowthParameters.m_lateralBudLightingFactor = 1															;
	//m_shootGrowthParameters.m_leafBudLightingFactor = 1																;
	//m_shootGrowthParameters.m_fruitBudLightingFactor = 1															;
	m_shootGrowthParameters.m_apicalControl = 0.75;

	m_shootGrowthParameters.m_apicalControl =
		1.0f + 0.75 * glm::exp(-0.159999996 * treeModel.m_age);

	m_shootGrowthParameters.m_apicalDominance = [=](const Node<InternodeGrowthData>& internode)
		{
			return 0.349999994 * glm::exp(0 * treeModel.m_age);
		};
	m_shootGrowthParameters.m_apicalDominanceDistanceFactor = 0.999000013;
	m_shootGrowthParameters.m_leafVigorRequirement = 0.100000001;
	m_shootGrowthParameters.m_fruitVigorRequirement = 0.100000001;
	m_shootGrowthParameters.m_internodeVigorRequirement = 1;
	m_shootGrowthParameters.m_vigorRequirementAggregateLoss = 0.949999988;
	m_shootGrowthParameters.m_lowBranchPruning = 0.0500000007;

	m_shootGrowthParameters.m_sagging = [=](const Node<InternodeGrowthData>& internode)
		{
			const auto& m_saggingFactorThicknessReductionMax = glm::vec3(0.00499999989, 3, 0.300000012);
			const float newSagging = glm::min(
				0.300000012,
				m_saggingFactorThicknessReductionMax.x *
				(internode.m_data.m_descendentTotalBiomass + internode.m_data.m_extraMass) /
				glm::pow(
					internode.m_info.m_thickness /
					//endnodethickness:
					0.00200000009,
					m_saggingFactorThicknessReductionMax.y));
			return glm::max(internode.m_data.m_sagging, newSagging);
		};

	m_shootGrowthParameters.m_maxLeafSize = { 0.0500000007, 0.5, 0.0500000007 };
	m_shootGrowthParameters.m_leafPositionVariance = 0.5;
	//m_shootGrowthParameters.m_leafRandomRotation = 10																;
	m_shootGrowthParameters.m_leafDamage = [=](const Node<InternodeGrowthData>& internode)
		{
			const auto& internodeData = internode.m_data;
			float leafDamage = 0.0f;
			//shootGrowthParameters.m_leafChlorophyllSynthesisFactorTemperature is 65
			if (m_climateModel.m_time - glm::floor(m_climateModel.m_time) > 0.5f && internodeData.m_temperature < 65)
			{
				//4 is m_leafChlorophyllLoss
				leafDamage += 4;
			}
			return leafDamage;
		};
	m_shootGrowthParameters.m_leafFallProbability = [=](const Node<InternodeGrowthData>& internode)
		{
			return 3;
		};
	m_shootGrowthParameters.m_maxFruitSize = { 0.0350000001, 0.0350000001, 0.0350000001 };
	m_shootGrowthParameters.m_fruitPositionVariance = 0.5;
	//m_shootGrowthParameters.m_fruitRandomRotation = 10																;
	m_shootGrowthParameters.m_fruitFallProbability = [=](const Node<InternodeGrowthData>& internode)
		{
			return 3;
		};
	m_shootGrowthParameters.m_pruningFactor = [=](const Node<InternodeGrowthData>& internode)
		{
			return 0.0;
		};

	//bool Grow(float deltaTime, const glm::mat4 & globalTransform, SoilModel & soilModel, ClimateModel & climateModel,
	//const RootGrowthController & rootGrowthParameters, const ShootGrowthController & shootGrowthParameters);
	MGlobal::displayInfo("Abt to Grow, stand back!!");
	treeModel.Grow(.1f, glm::mat4(), m_soilModel, m_climateModel, m_rootGrowthParameters, m_shootGrowthParameters);

	roots = treeModel.RefShootSkeleton().RefSortedNodeList().size();
	MGlobal::displayInfo("Run 1: ");
	MGlobal::displayInfo(MString(std::to_string(roots).c_str()));

	treeModel.Grow(.1f, glm::mat4(), m_soilModel, m_climateModel, m_rootGrowthParameters, m_shootGrowthParameters);

	roots = treeModel.RefShootSkeleton().RefSortedNodeList().size();
	MGlobal::displayInfo("Run 2: ");
	MGlobal::displayInfo(MString(std::to_string(roots).c_str()));

	treeModel.Grow(.1f, glm::mat4(), m_soilModel, m_climateModel, m_rootGrowthParameters, m_shootGrowthParameters);

	roots = treeModel.RefShootSkeleton().RefSortedNodeList().size();
	MGlobal::displayInfo("Final Root num: ");
	MGlobal::displayInfo(MString(std::to_string(roots).c_str()));

	return MStatus::kSuccess;
}

