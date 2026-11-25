#pragma once
#include <string>
#include <vector>
#include "Biome.h"

class World_Creation {
private:
    std::string m_worldName;
    int m_worldEra;
    unsigned long long m_worldEnergy;
    unsigned long long m_tickCount;
    std::vector<Biome> m_biomes;

public:
    World_Creation(const std::string& name, unsigned long long startingEnergy);

    void AdvanceTime(int ticks = 1);
    bool CreateBiome(const Biome& newBiome, unsigned long long cost);
    bool UpgradeBiome(int index);
    bool SeedSentientRace(int biomeIndex, const Race& newRace, unsigned long long cost);
    void DisplayStatus() const;
};


