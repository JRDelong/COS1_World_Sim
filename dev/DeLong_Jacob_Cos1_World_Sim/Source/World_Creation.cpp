#include "World_Creation.h"
#include <iostream>

World_Creation::World_Creation(const std::string& name, unsigned long long startingOP)
    : m_worldName(name), m_worldEra(1), m_overseerPoints(startingOP), m_tickCount(0) {
    InitializeAchievements();
}

void World_Creation::AdvanceTime(int ticks) {
    for (int t = 0; t < ticks; ++t) {
        m_tickCount++;
        for (size_t i = 0; i < m_biomes.size(); ++i) {
            m_biomes[i].ProcessTick();
        }
        CheckAchievements();
    }
    std::cout << "\n>> Time advanced by " << ticks << " epoch(s). Overseer Points: " << m_overseerPoints << " OP\n";
}

bool World_Creation::CreateBiome(const Biome& newBiome, unsigned long long cost) {
    if (m_overseerPoints < cost) {
        std::cout << "\n[!] Not enough Overseer Points! Need " << cost << " OP.\n";
        return false;
    }
    m_overseerPoints -= cost;
    m_biomes.push_back(newBiome);
    std::cout << "\n[+] Manifested " << newBiome.GetName() << " [" << newBiome.GetTrait() << "]!\n";
    CheckAchievements();
    return true;
}

bool World_Creation::UpgradeBiome(int index) {
    if (m_biomes.empty()) {
        std::cout << "\n[!] No biomes exist yet. Manifest a biome first!\n";
        return false;
    }
    if (index < 0 || index >= static_cast<int>(m_biomes.size())) {
        std::cout << "\n[!] Invalid biome index. Valid range: 0 to " << (m_biomes.size() - 1) << ".\n";
        return false;
    }

    unsigned long long cost = m_biomes[index].GetUpgradeCost();
    if (m_overseerPoints < cost) {
        std::cout << "\n[!] Insufficient Overseer Points. Upgrade requires " << cost << " OP.\n";
        return false;
    }

    m_overseerPoints -= cost;
    m_biomes[index].UpgradeTier();
    std::cout << "\n[+] Upgraded " << m_biomes[index].GetName() << " to Tier " << m_biomes[index].GetTier() << "!\n";
    CheckAchievements();
    return true;
}

bool World_Creation::SeedSentientRace(int biomeIndex, const Race& newRace, unsigned long long cost) {
    if (m_biomes.empty()) {
        std::cout << "\n[!] No biomes exist yet. Manifest a biome first!\n";
        return false;
    }
    if (biomeIndex < 0 || biomeIndex >= static_cast<int>(m_biomes.size())) {
        std::cout << "\n[!] Invalid biome index. Valid range: 0 to " << (m_biomes.size() - 1) << ".\n";
        return false;
    }
    if (m_biomes[biomeIndex].HasSentientRace()) {
        std::cout << "\n[!] That biome already hosts a sentient civilization!\n";
        return false;
    }
    if (m_overseerPoints < cost) {
        std::cout << "\n[!] Not enough Overseer Points! Awakening " << newRace.GetName()
            << " requires " << cost << " OP.\n";
        return false;
    }

    m_overseerPoints -= cost;
    m_biomes[biomeIndex].SeedRace(newRace);
    std::cout << "\n[+] Sparked Sentient Life! The " << newRace.GetName()
        << " (" << newRace.GetTrait() << ") have awakened in "
        << m_biomes[biomeIndex].GetName() << "!\n";
    CheckAchievements();
    return true;
}

void World_Creation::DisplayStatus() const {
    std::cout << "\n======================================================\n";
    std::cout << " WORLD: " << m_worldName << " | EPOCH: " << m_tickCount << "\n";
    std::cout << " OVERSEER POINTS: " << m_overseerPoints << " OP\n";
    std::cout << "======================================================\n";

    if (m_biomes.empty()) {
        std::cout << " No biomes manifested yet. The world is barren.\n";
    }
    else {
        for (size_t i = 0; i < m_biomes.size(); ++i) {
            std::cout << " [" << i << "] " << m_biomes[i].GetName()
                << " (Tier " << m_biomes[i].GetTier() << " | Trait: " << m_biomes[i].GetTrait() << ")\n"
                << "     Bonus: " << m_biomes[i].GetBonusDesc() << " | Flaw: " << m_biomes[i].GetFlawDesc() << "\n"
                << "     Ecosystem Pop: " << m_biomes[i].GetWildLifePop() << "/" << m_biomes[i].GetPopulationCap()
                << " | Civ Power: " << m_biomes[i].CalculateBiomePower()
                << " | Upgrade Cost: " << m_biomes[i].GetUpgradeCost() << " OP\n"
                << "     Sentient Life: " << m_biomes[i].GetRaceSummary() << "\n";
        }
    }
    std::cout << "======================================================\n";
}

void World_Creation::DisplayAchievements() const {
    std::cout << "\n================ OVERSEER ACHIEVEMENTS ================\n";
    for (size_t i = 0; i < m_achievements.size(); ++i) {
        std::string status = m_achievements[i].unlocked ? "[UNLOCKED]" : "[LOCKED]  ";
        std::cout << " " << status << " " << m_achievements[i].title
            << " (+" << m_achievements[i].rewardOP << " OP)\n"
            << "            -> " << m_achievements[i].description << "\n";
    }
    std::cout << "=======================================================\n";
}


