#pragma once
#include <string>

class Race {
private:
    std::string m_name;
    std::string m_trait;
    std::string m_bonusDesc;
    std::string m_flawDesc;
    std::string m_preferredBiome;
    unsigned long long m_population;
    int m_civLevel;
    unsigned long long m_devPoints;
    int m_growthBonusPercent;
    int m_devSpeedPercent;
    int m_prosperityPercent; // Formerly energyYieldPercent; boosts Civilization Power

public:
    Race(const std::string& name,
        const std::string& trait,
        const std::string& bonusDesc,
        const std::string& flawDesc,
        const std::string& preferredBiome,
        int growthBonusPercent,
        int devSpeedPercent,
        int prosperityPercent);

    void ProcessTick(unsigned long long biomeHabitability, unsigned long long popCap);
    unsigned long long CalculateCivPower(const std::string& currentBiomeName) const;
    std::string GetCivEraName() const;

    // Getters
    std::string GetName() const { return m_name; }
    std::string GetTrait() const { return m_trait; }
    std::string GetBonusDesc() const { return m_bonusDesc; }
    std::string GetFlawDesc() const { return m_flawDesc; }
    std::string GetPreferredBiome() const { return m_preferredBiome; }
    unsigned long long GetPopulation() const { return m_population; }
    int GetCivLevel() const { return m_civLevel; }
};



