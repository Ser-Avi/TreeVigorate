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

void TreeCmd::RegisterMELCommands() {
	const char* treeUIcmd = R"(
global proc createTreeUI() {
	if (`window -exists treeUI`) {
        deleteUI treeUI;
    }
    
    window -title "Tree Controls" -widthHeight 250 220 treeUI;
    
    columnLayout -adjustableColumn true;
    
    // Float slider for more interactive control
    floatSliderGrp -label "Growth Rate" 
                  -field true 
                  -minValue 1.0 
                  -maxValue 100.0 
                  -value 50.0 
                  -step 0.5
                  -dragCommand "updateRate" 
                  -changeCommand "updateRate"
				  -annotation "Controls how fast each growth is calculated"
                  rateSlider;
    
    separator -height 10;

	intSliderGrp -label "Growth Amount" 
					  -field true 
					  -minValue 0 
					  -maxValue 1000 
					  -value 0 
					  -step 1
					  -dragCommand "updateGrowth" 
					  -changeCommand "updateGrowth"
					  -annotation "Controls how much the tree grows with each calculation"
					  growthSlider;
    
    separator -height 10;

	floatSliderGrp -label "Radius Adjuster" 
					  -field true 
					  -minValue 0.2
					  -maxValue 50.0 
					  -value 1.0 
					  -step 0.2
					  -dragCommand "updateRad" 
					  -changeCommand "updateRad"
				      -annotation "Adjusts tree radius with this multiplier"
					  radSlider;
    
    separator -height 10;

	// button to initialize growth
	button	-label "Grow Once" 
			-command "toggleGrow"
			-backgroundColor 0.2 0.6 0.2;
	
	separator -height 10;

	button	-label "Play"
			-command "playPause"
			-backgroundColor 0.8 0.3 0.3
			playButton;

	separator -height 10;

	// Display total growth time
	text -label "Total Grow Time:";
    textField -editable false -text (`getAttr TN1.growTime`) growTimeField;

	separator - style "none" - height 15;
    separator - style "in" - height 5;
    separator - style "none" - height 15;
	text -label "Node Editing";
	separator - style "in" -height 5;

	// Display total flow node num
	text -label "Number of Tree Nodes:";
    textField -editable false -text (`getAttr TN1.numNodes`) nodeNumField;

	// Edit this specific node
	text -label "Selected Node";
	intField -minValue 0 -maxValue 100 -value 0 -changeCommand "updateSelectedNode" nodeSelectField;
	
	floatSliderGrp -label "Edit Growth Potential" 
					  -field true 
					  -minValue 0.0
					  -maxValue 50.0 
					  -value 1.0 
					  -step 0.2
					  -dragCommand "updateVigor" 
					  -changeCommand "updateVigor"
				      -annotation "Adjusts the selected node's growth potential"
					  vigorSlider;
    
    // Add cleanup when window closes
    scriptJob -uiDeleted "treeUI" "onTreeUIClose";

	showWindow treeUI;
}

global proc updateVigor() {
	float $value = `floatSliderGrp -query -value vigorSlider`;
	print("New vigor: " + $value);
}

global proc updateSelectedNode() {
	int $uiVal = `intField -query -value nodeSelectField`;
	setAttr TN1.selectedNode $uiVal;
	print("Selected node: " + $uiVal);
	// display the selected node's vigor
	refresh -force;
	
	int $test = `getAttr TN1.selectedNode`;
	print("Test: " + $test);

	float $vigor = `getAttr TN1.selectedVigor`;
	floatSliderGrp -edit -value $vigor vigorSlider;
}

global proc updateGrowTime() {
	float $curr = `getAttr TN1.growTime`;
    textField -edit -text $curr growTimeField;
}

global proc updateNodeNum() {
	int $curr = `getAttr TN1.numNodes`;
    textField -edit -text $curr nodeNumField;
	// setting the maximum value of node selection to this
	intField -edit -maxValue $curr nodeSelectField;
}

global proc updateRate() {
    float $value = `floatSliderGrp -query -value rateSlider`;
	setAttr TN1.deltaTime ( 1 / $value);
}

global proc updateRad() {
    float $value = `floatSliderGrp -query -value radSlider`;
	setAttr TN1.radius $value;
}

global proc updateGrowth() {
    float $value = `intSliderGrp -query -value growthSlider`;
	setAttr TN1.numGrows $value;
}

global proc toggleGrow() {
	int $toggle = `getAttr TN1.makeGrow`;
	$toggle = 1 - $toggle;
	setAttr TN1.makeGrow $toggle;

	refresh -force;
	updateGrowTime;
	updateNodeNum;
}

global proc playPause() {
    // Check if we're currently playing
    int $isPlaying = `optionVar -exists "growPlaying"` ? `optionVar -q "growPlaying"` : 0;
    
    if ($isPlaying) {
        // Pause the playback
        int $jobNum = `optionVar -q "treeJob"`;
        scriptJob -kill $jobNum -force;
        optionVar -remove "treeJob";
        optionVar -iv "growPlaying" 0;
        
        // Update button label
        button -edit -label "Play" -backgroundColor 0.8 0.3 0.3 playButton;
        print "Playback paused\n";
    } else {
        // Start playing
        int $jobNum = `scriptJob -event "idle" "toggleGrow"`;
        optionVar -iv "treeJob" $jobNum;
        optionVar -iv "growPlaying" 1;
        
        // Update button label
        button -edit -label "Pause" -backgroundColor 0.3 0.4 0.6 playButton;
        print "Playback started\n";
    }
}

global proc onTreeUIClose() {
    // Cleanup any running playback when window closes
    if (`optionVar -exists "growPlaying"` && `optionVar -q "growPlaying"`) {
        int $jobNum = `optionVar -q "treeJob"`;
        scriptJob -kill $jobNum -force;
        optionVar -remove "treeJob";
        optionVar -remove "growPlaying";
    }
}
	)";
	
	const char* windowCommand = R"(
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

global proc generateTreeFromPreset (string $tree) {
	createTreeNode($tree);
	// Close the UI Window
    if (`window -exists generateTreeWindow`) {
        deleteUI generateTreeWindow;
    }
}

global proc createSunLoc(string $tree) {
	// deleting old one if it exists
    if (`objExists "sunLoc"`) delete "sunLoc";
    // create new locator
    spaceLocator -name "sunLoc";
    setAttr sunLoc.translate 0 10 0; // Default to Up
    connectAttr sunLoc.translate ($tree + ".sunDir");

	// clamping sun location to always be positive on y-axis
	transformLimits -ty 0 1000 -ety 1 0 "sunLoc";
}

global proc createTreeNode(string $file) {
	createNode transform -n TSys1;
	createNode transform -n TSys2;			// the 2s are for the highlighted node
	createNode mesh -n TShape1 -p TSys1;
	createNode mesh -n TShape2 -p TSys2;
	sets -add initialShadingGroup TShape1;

	// making second shape have a red shade for highlighting
	shadingNode -asShader lambert -n "RedShader";
	setAttr RedShader.color -type double3 1 0 0;
	shadingNode -asUtility shadingEngine -n "RedShaderSG";
	connectAttr -f "RedShader.outColor" "RedShaderSG.surfaceShader";
	sets -e -forceElement "RedShaderSG" "TShape2";

	string $treeNode = `createNode TreeNode -n TN1`;
    if($file != "") {
        eval("setAttr -type \"string\" TN1.treeDataFile \"" + $file + "\"");
    }
	connectAttr TN1.outputMesh TShape1.inMesh;
	connectAttr TN1.outputNodeMesh TShape2.inMesh;

	// creating Sun Direction locator
	createSunLoc($treeNode);
	createTreeUI;
};

global proc generateTree() {
    //string $treeData = `textField -q -text treeDataField`;
    string $treeSpecies = `textField -q -text treeSpeciesField`;

    //print ("Generating tree with:\nTree Data: " + $treeData + "\nTree Species: " + $treeSpecies + "\n");
    print("Generating tree node");
    createTreeNode($treeSpecies);

    // Close the UI Window
    if (`window -exists generateTreeWindow`) {
        deleteUI generateTreeWindow;
    }
    
}

// Create the UI Window
global proc createTreeGeneratorWindow() {
    if (`window -exists generateTreeWindow`) {
        deleteUI generateTreeWindow;
    }

    window -title "Generate Tree" -widthHeight 350 230 generateTreeWindow;
    columnLayout -adjustableColumn true;

    // Add spacing at the top
    separator -style "none" -height 10;
	text - label "Tree Presets";
    separator -style "none" -height 10;

	// row for buttons
	rowLayout	-numberOfColumns 2 
				-columnWidth2 175 175 
				-columnAttach2 "both" "both"
				-columnOffset2 0 0;

		button	- label "Birch Tree"
				- height 50
				- width 175
				- command "generateTreeFromPreset \"Birch\"";

		button	- label "Spruce Tree"
				- height 50
				- width 175
				- command "generateTreeFromPreset \"Spruce\"";
	setParent..;


    // Add some space
    separator - style "none" - height 15;
    separator - style "in" - height 5;
    separator - style "none" - height 15;

    // Tree Species File Selection
	text - label "Select Custom Tree File";
    separator - style "none" - height 5;
    rowLayout - numberOfColumns 2 - columnWidth2 290 60;
		textField - editable false - width 190 treeSpeciesField;
		button - label "Browse" - command("browseTreeSpeciesFile");
    setParent ..;

    separator -style "none" -height 10;

    // Generate Tree Button
    button - label "Generate Tree From File" - command("generateTree") - height 50 -width 290;
    showWindow generateTreeWindow;
}
)";

	const char* iterationCmds = R"(
		global proc createTreeGrowthUI() {
			// Window name
			string $windowName = "treeGrowthController";
    
			// Delete if window already exists
			if (`window -exists $windowName`) {
				deleteUI $windowName;
			}
    
			// Create window
			window -title "Tree Growth Controller" -widthHeight 400 150 $windowName;
    
			// Main layout
			columnLayout -adjustableColumn true -columnAttach "both" 5;
    
			// Text label
			text -label "Iterate Tree Growth" -align "center" -font "boldLabelFont";
    
			// Divider
			separator -height 20;
    
			// Slider for number of growth iterations
			rowLayout -numberOfColumns 3 -columnWidth3 100 200 50;
				text -label "Iterations:";
				intSlider -minValue 1 -maxValue 100 -value 10 -step 1 
						 -dragCommand "updateIterationField" 
						 -changeCommand "updateIterationField"
						 "iterationSlider";
				intField -value 10 -minValue 1 -maxValue 100 -step 1
						-changeCommand "updateIterationSlider"
						"iterationField";
			setParent..;
    
			// Button to start growth animation
			button -label "Animate Growth" 
				   -command "executeTreeGrowth"
				   -height 40
				   -backgroundColor 0.2 0.6 0.2;
    
			// Help text
			text -label "Select your TreeNode first" -font "smallPlainLabelFont";
    
			// Show the window
			showWindow $windowName;
		}

		// Update field when slider changes
		global proc updateIterationField() {
			int $val = `intSlider -q -value iterationSlider`;
			intField -e -value $val iterationField;
		}

		// Update slider when field changes
		global proc updateIterationSlider() {
			int $val = `intField -q -value iterationField`;
			intSlider -e -value $val iterationSlider;
		}

		// Execute the growth animation
		global proc executeTreeGrowth() {
			// Finding tree node with hardcoded name
			string $treeNode = "TN" + `match "[0-9]+$" "TSys1"`;
			if (!`objExists $treeNode`) {
				error "TreeNode not found";
				return;
			}
    
			// Get iteration value
			int $iterations = `intSlider -q -value iterationSlider`;
    
			// Animate through each frame
			for ($i = 1; $i <= $iterations; $i++) {
				int $toggle = `getAttr ($treeNode + ".makeGrow")`;
				$toggle = 1 - $toggle;
				setAttr ($treeNode + ".makeGrow") $toggle;
				refresh -force;
			}
			print ("Growth animation completed for " + $treeNode + "\n");
		}
	)";

	const char* menuCommand = R"(
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
menuItem
	-label "Tree Controls"
	-command("createTreeUI")
		systemItem1;
menuItem
	-label "Iterate Growth"
	-command("createTreeGrowthUI")
		systemItem2;
)";

	MGlobal::executeCommand(treeUIcmd);
	MGlobal::executeCommand(windowCommand);
	MGlobal::executeCommand(menuCommand);
	MGlobal::executeCommand(iterationCmds);
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

