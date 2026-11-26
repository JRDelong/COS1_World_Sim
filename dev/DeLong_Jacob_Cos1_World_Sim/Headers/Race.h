#pragma once

#include "Biome.h"
#include <string>
#include <vector>

struct RacePreset {
    std::string factionName;
    std::string speciesName;
    BiomeType preferredBiome = BiomeType::Plains;
    int startingAggression = 30;
    int startingAdaptability = 50;
    int costOP = 300;
    std::string summary;
};

class Race {
public:
    static std::vector<RacePreset> GetStarterCatalog();
};




