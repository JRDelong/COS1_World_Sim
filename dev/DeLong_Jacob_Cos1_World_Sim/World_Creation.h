#pragma once
#include <string>
#include <vector>
#include "Biome.h"

struct Achievement {
    std::string title;
    std::string description;
    unsigned long long rewardOP;
    bool unlocked;
};

class World_Creation {
private:
    std::string m_worldName;
    int m_worldEra;
    unsigned long long m_overseerPoints;
    unsigned long long m_tickCount;
    std::vector<Biome> m_biomes;
    std::vector<Achievement> m_achievements;

    void InitializeAchievements();
    void CheckAchievements();

public:
    World_Creation(const std::string& name, unsigned long long startingOP);

    void AdvanceTime(int ticks = 1);
    bool CreateBiome(const Biome& newBiome, unsigned long long cost);
    bool UpgradeBiome(int index);
    bool SeedSentientRace(int biomeIndex, const Race& newRace, unsigned long long cost);
    void DisplayStatus() const;
    void DisplayAchievements() const;
};



