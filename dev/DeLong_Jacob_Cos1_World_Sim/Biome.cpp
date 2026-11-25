#include "Biome.h"

Biome::Biome(const std::string& name,
    const std::string& trait,
    const std::string& bonusDesc,
    const std::string& flawDesc,
    unsigned long long habitability,
    int energyModifierPercent,
    int capModifierPercent)
    : m_name(name),
    m_trait(trait),
    m_bonusDesc(bonusDesc),
    m_flawDesc(flawDesc),
    m_tier(1),
    m_wildLifePop(10),
    m_habitability(habitability),
    m_energyModifierPercent(energyModifierPercent),
    m_capModifierPercent(capModifierPercent) {
}

unsigned long long Biome::GetPopulationCap() const {
    unsigned long long cap = 1000;
    for (int i = 1; i < m_tier; ++i) {
        cap *= 10;
    }
    return (cap * m_capModifierPercent) / 100;
}

void Biome::ProcessTick() {
    unsigned long long cap = GetPopulationCap();

    // Grow baseline wildlife/ecosystem
    if (m_wildLifePop < cap) {
        unsigned long long growth = (m_wildLifePop * m_habitability) / 100;
        if (growth == 0) growth = 1;
        m_wildLifePop += growth;
        if (m_wildLifePop > cap) {
            m_wildLifePop = cap;
        }
    }

    // Grow sentient civilization if present
    if (!m_races.empty()) {
        m_races.front().ProcessTick(m_habitability, cap);
    }
}

unsigned long long Biome::CalculateEnergyYield() const {
    unsigned long long yield = (m_wildLifePop / 5) * m_tier;
    if (!m_races.empty()) {
        yield += m_races.front().CalculateEnergyYield(m_name);
    }
    return (yield * m_energyModifierPercent) / 100;
}

unsigned long long Biome::GetUpgradeCost() const {
    unsigned long long cost = 500;
    for (int i = 1; i < m_tier; ++i) {
        cost *= 4;
    }
    return cost;
}

bool Biome::UpgradeTier() {
    m_tier++;
    return true;
}

bool Biome::SeedRace(const Race& newRace) {
    if (!m_races.empty()) {
        return false;
    }
    m_races.push_back(newRace);
    return true;
}

std::string Biome::GetRaceSummary() const {
    if (m_races.empty()) {
        return "None (Uninhabited Wilderness)";
    }
    const Race& r = m_races.front();
    std::string harmony = (r.GetPreferredBiome() == m_name) ? " [Preferred Biome +25%]" : "";
    return r.GetName() + " [" + r.GetTrait() + "] - " + r.GetCivEraName() +
        " (Pop: " + std::to_string(r.GetPopulation()) + ")" + harmony +
        "\n       Race Bonus: " + r.GetBonusDesc() + " | Race Flaw: " + r.GetFlawDesc();
}


