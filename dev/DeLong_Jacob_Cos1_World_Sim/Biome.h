#pragma once
#include <string>




class Biome
{
private:
	std::string m_name;
	int m_tier;
	unsigned long long m_population;
	unsigned long long m_habitability;

public:
	Biome(const std::string& name, unsigned long long habitability);

	void ProcessTick();
	unsigned long long CalculateEnergyYield() const;
	unsigned long long GetUpgradeCost() const;
	unsigned long long GetPopulationCap() const;
	bool UpgradeTier();

	std::string GetName() const {
		return m_name;
	}
	int GetTier() const {
		return m_tier;
	}
	unsigned long long GetPopulation() const {
		return m_population;
	}
};

