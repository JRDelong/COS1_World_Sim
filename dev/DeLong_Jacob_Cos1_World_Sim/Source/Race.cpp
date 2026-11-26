#include "Race.h"

std::vector<RacePreset> Race::GetStarterCatalog() {
    std::vector<RacePreset> catalog;
    catalog.push_back({ "Sylvan Kin", "Elves", BiomeType::Forest, 20, 45, 300, "Thrives in Forests; peaceful gatherers" });
    catalog.push_back({ "Stoneborn Clan", "Dwarves", BiomeType::Mountains, 35, 45, 300, "Thrives in Mountains; sturdy builders" });
    catalog.push_back({ "Plainswalkers", "Humans", BiomeType::Plains, 30, 65, 275, "Thrives in Plains; highly adaptable settlers" });
    catalog.push_back({ "Tideborn Collective", "Nereids", BiomeType::Wetlands, 25, 50, 300, "Thrives in Wetlands; steady traders" });
    catalog.push_back({ "Sunforged Nomads", "Dune-Kin", BiomeType::Desert, 45, 70, 275, "Thrives in Deserts; hardy, aggressive survivors" });
    return catalog;
}


