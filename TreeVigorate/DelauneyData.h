#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <unordered_map>
class DelauneyData {
public:
	std::vector<int> idx;
	std::vector<int> polarHullIdx;
	std::unordered_map<int, int> vertIdToParticleIndex;
	std::unordered_map<int, int> particleIndexToVertId;


	DelauneyData(std::vector<int> idx, std::vector<int> polarHullIdx, std::unordered_map<int, int> vertIdToParticleIndex, std::unordered_map<int, int> particleIndexToVertId);
	DelauneyData();
};