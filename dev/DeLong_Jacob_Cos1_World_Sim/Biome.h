#pragma once
#include <string>
#include <vector>
#include "Race.h"

class Biome {
private:
    std::string m_name;
    std::string m_trait;
    std::string m_bonusDesc;
    std::string m_flawDesc;
    int m_tier;
    unsigned long long m_wildLifePop;
    unsigned long long m_habitability;
    int m_energyModifierPercent; // e.g., 150 = +50% energy yield, 80 = -20% energy yield
    int m_capModifierPercent;    // e.g., 100 = normal cap, 60 = -40% pop cap
    std::vector<Race> m_races;

public:
    Biome(const std::string& name,
        const std::string& trait,
        const std::string& bonusDesc,
        const std::string& flawDesc,
        unsigned long long habitability,
        int energyModifierPercent,
        int capModifierPercent);

    void ProcessTick();
    unsigned long long CalculateEnergyYield() const;
    unsigned long long GetUpgradeCost() const;
    unsigned long long GetPopulationCap() const;
    bool UpgradeTier();

    // Sentient Life Methods
    bool HasSentientRace() const { return !m_races.empty(); }
    bool SeedRace(const Race& newRace;
    std::string GetRaceSummary() const;

    // Getters
    std::string GetName() const { return m_name; }
    std::string GetTrait() const { return m_trait; }
    std::string GetBonusDesc() const { return m_bonusDesc; }
    std::string GetFlawDesc() const { return m_flawDesc; }
    int GetTier() const { return m_tier; }
    unsigned long long GetWildLifePop() const { return m_wildLifePop; }
};


