#include "Race.h"

Race::Race(const std::string& name,
    const std::string& trait,
    const std::string& bonusDesc,
    const std::string& flawDesc,
    const std::string& preferredBiome,
    int growthBonusPercent,
    int devSpeedPercent,
    int energyYieldPercent)
    : m_name(name),
    m_trait(trait),
    m_bonusDesc(bonusDesc),
    m_flawDesc(flawDesc),
    m_preferredBiome(preferredBiome),
    m_population(50),
    m_civLevel(1),
    m_devPoints(0),
    m_growthBonusPercent(growthBonusPercent),
    m_devSpeedPercent(devSpeedPercent),
    m_energyYieldPercent(energyYieldPercent) {
}

void Race::ProcessTick(unsigned long long biomeHabitability, unsigned long long popCap) {
    // 1. Population Growth (clamped to minimum 5% so negative flaws never stall completely)
    int effectiveRate = static_cast<int>(biomeHabitability) + m_growthBonusPercent;
    if (effectiveRate < 5) {
        effectiveRate = 5;
    }

    if (m_population < popCap) {
        unsigned long long growth = (m_population * static_cast<unsigned long long>(effectiveRate)) / 100;
        if (growth == 0) growth = 1;
        m_population += growth;
        if (m_population > popCap) {
            m_population = popCap;
        }
    }

    // 2. Civilization Development
    unsigned long long baseDev = (m_population / 20) + 5;
    unsigned long long devGain = (baseDev * static_cast<unsigned long long>(m_devSpeedPercent)) / 100;
    if (devGain == 0) devGain = 1;
    m_devPoints += devGain;

    // 3. Era Advancement
    unsigned long long nextEraThreshold = 500;
    for (int i = 1; i < m_civLevel; ++i) {
        nextEraThreshold *= 5;
    }

    if (m_devPoints >= nextEraThreshold && m_civLevel < 5) {
        m_devPoints -= nextEraThreshold;
        m_civLevel++;
    }
}

unsigned long long Race::CalculateEnergyYield(const std::string& currentBiomeName) const {
    unsigned long long baseYield = (m_population / 2) * static_cast<unsigned long long>(m_civLevel);
    unsigned long long modifiedYield = (baseYield * static_cast<unsigned long long>(m_energyYieldPercent)) / 100;

    // +25% Harmony Bonus when placed in their Preferred Biome
    if (currentBiomeName == m_preferredBiome) {
        modifiedYield = (modifiedYield * 125) / 100;
    }
    return modifiedYield;
}

std::string Race::GetCivEraName() const {
    switch (m_civLevel) {
    case 1: return "Tribal Era";
    case 2: return "Agrarian Era";
    case 3: return "Metallurgy Era";
    case 4: return "Arcane-Tech Era";
    case 5: return "Ascendant Era";
    default: return "Unknown Era";
    }
}
