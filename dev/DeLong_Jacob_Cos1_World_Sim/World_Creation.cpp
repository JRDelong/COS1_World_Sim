#include "World_Creation.h"
#include <iostream>

World_Creation::World_Creation(const std::string& name, unsigned long long startingOP)
    : m_worldName(name), m_worldEra(1), m_overseerPoints(startingOP), m_tickCount(0) {
    InitializeAchievements();
}

void World_Creation::InitializeAchievements() {
    m_achievements = {
        { "First Spark",       "Manifest your first biome.",                         250,  false },
        { "Breath of Mind",    "Awaken your first sentient race.",                   400,  false },
        { "Perfect Harmony",   "Awaken a sentient race in its Preferred Biome.",     500,  false },
        { "Thriving Ecosystem","Reach 1,000 total wildlife population.",             350,  false },
        { "Growing Tribes",    "Reach 500 total sentient population.",               500,  false },
        { "Agrarian Dawn",     "Advance any race to the Agrarian Era (Level 2).",    600,  false },
        { "Age of Forges",     "Advance any race to the Metallurgy Era (Level 3).",  1000, false },
        { "Diverse Genesis",   "Manifest at least 3 distinct biomes.",               800,  false },
        { "World Shaping",     "Upgrade any biome to Tier 2.",                       750,  false },
        { "Arcane Renaissance","Advance any race to the Arcane-Tech Era (Level 4).", 1800, false },
        { "Ascendant Realm",   "Advance any race to the Ascendant Era (Level 5).",   3500, false }
    };
}

void World_Creation::CheckAchievements() {
    unsigned long long totalWildPop = 0;
    unsigned long long totalSentientPop = 0;
    int sentientRaceCount = 0;
    bool hasHarmony = false;
    int maxCivLevel = 0;
    int maxBiomeTier = 0;

    for (size_t i = 0; i < m_biomes.size(); ++i) {
        totalWildPop += m_biomes[i].GetWildLifePop();
        totalSentientPop += m_biomes[i].GetSentientPop();
        if (m_biomes[i].HasSentientRace()) {
            sentientRaceCount++;
        }
        if (m_biomes[i].IsRaceInPreferredBiome()) {
            hasHarmony = true;
        }
        if (m_biomes[i].GetSentientCivLevel() > maxCivLevel) {
            maxCivLevel = m_biomes[i].GetSentientCivLevel();
        }
        if (m_biomes[i].GetTier() > maxBiomeTier) {
            maxBiomeTier = m_biomes[i].GetTier();
        }
    }

    bool conditions[11] = {
        !m_biomes.empty(),          // 0: First Spark
        sentientRaceCount >= 1,     // 1: Breath of Mind
        hasHarmony,                 // 2: Perfect Harmony
        totalWildPop >= 1000,       // 3: Thriving Ecosystem
        totalSentientPop >= 500,    // 4: Growing Tribes
        maxCivLevel >= 2,           // 5: Agrarian Dawn
        maxCivLevel >= 3,           // 6: Age of Forges
        m_biomes.size() >= 3,       // 7: Diverse Genesis
        maxBiomeTier >= 2,          // 8: World Shaping
        maxCivLevel >= 4,           // 9: Arcane Renaissance
        maxCivLevel >= 5            // 10: Ascendant Realm
    };

    for (size_t i = 0; i < m_achievements.size(); ++i) {
        if (!m_achievements[i].unlocked && conditions[i]) {
            m_achievements[i].unlocked = true;
            m_overseerPoints += m_achievements[i].rewardOP;
            std::cout << "\n[*** ACHIEVEMENT UNLOCKED ***] " << m_achievements[i].title
                << " - " << m_achievements[i].description
                << " (+" << m_achievements[i].rewardOP << " Overseer Points!)\n";
        }
    }
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


