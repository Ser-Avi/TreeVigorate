#pragma once
#include <vector>
#include <glm/glm.hpp>
class DelauneyData {
public:
	std::vector<int> idx;
	std::vector<int> hullIdx;

	DelauneyData(std::vector<int> idx, std::vector<int> hullIdx);
};