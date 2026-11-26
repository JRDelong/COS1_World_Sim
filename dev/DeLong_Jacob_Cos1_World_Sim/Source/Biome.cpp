#include "Biome.h"

std::vector<BiomePreset> Biome::GetStarterCatalog() {
    std::vector<BiomePreset> catalog;
    catalog.push_back({ "Emerald Reach", BiomeType::Forest, 80, 40, 200, "High food regen, moderate timber" });
    catalog.push_back({ "Sunlit Steppe", BiomeType::Plains, 75, 35, 200, "Balanced food & open frontier" });
    catalog.push_back({ "Ironcrag Peaks", BiomeType::Mountains, 25, 90, 250, "High material regen, low food" });
    catalog.push_back({ "Misty Wetlands", BiomeType::Wetlands, 65, 45, 225, "Steady food & marsh resources" });
    catalog.push_back({ "Ashen Dunes", BiomeType::Desert, 15, 50, 175, "Harsh climate, moderate minerals" });
    return catalog;
}

std::string Biome::ToString(BiomeType type) {
    switch (type) {
    case BiomeType::Forest:    return "Forest";
    case BiomeType::Plains:    return "Plains";
    case BiomeType::Mountains: return "Mountains";
    case BiomeType::Desert:    return "Desert";
    case BiomeType::Wetlands:  return "Wetlands";
    }
    return "Unknown";
}

int Biome::GetBaseFoodRegen(BiomeType type) {
    switch (type) {
    case BiomeType::Forest:    return 7;
    case BiomeType::Plains:    return 7;
    case BiomeType::Mountains: return 2;
    case BiomeType::Desert:    return 1;
    case BiomeType::Wetlands:  return 5;
    }
    return 4;
}

int Biome::GetBaseMaterialRegen(BiomeType type) {
    switch (type) {
    case BiomeType::Forest:    return 3;
    case BiomeType::Plains:    return 2;
    case BiomeType::Mountains: return 6;
    case BiomeType::Desert:    return 3;
    case BiomeType::Wetlands:  return 3;
    }
    return 2;
}



