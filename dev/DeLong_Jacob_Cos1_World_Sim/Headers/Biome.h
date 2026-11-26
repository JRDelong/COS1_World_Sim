#pragma once

#include <string>
#include <vector>

enum class BiomeType {
    Forest,
    Plains,
    Mountains,
    Desert,
    Wetlands
};

struct BiomePreset {
    std::string name;
    BiomeType type = BiomeType::Plains;
    int initialFood = 50;
    int initialMaterials = 50;
    int costOP = 200;
    std::string summary;
};

class Biome {
public:
    static std::vector<BiomePreset> GetStarterCatalog();
    static std::string ToString(BiomeType type);
    static int GetBaseFoodRegen(BiomeType type);
    static int GetBaseMaterialRegen(BiomeType type);
};




