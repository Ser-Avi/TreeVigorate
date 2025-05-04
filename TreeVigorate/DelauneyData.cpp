#include "DelauneyData.h"
#include <unordered_map>

DelauneyData::DelauneyData(std::vector<int> idx, std::vector<int> hullIdx, std::unordered_map<int,int> vertIdToParticleIndex, std::unordered_map<int, int> particleIndexToVertId) : idx(idx), polarHullIdx(hullIdx), vertIdToParticleIndex(vertIdToParticleIndex), particleIndexToVertId(particleIndexToVertId) {}
DelauneyData::DelauneyData() {}