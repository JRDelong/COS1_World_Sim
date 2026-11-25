#include "World_Creation.h"
#include <iostream>


World_Creation::World_Creation(const std::string& name, unsigned long long startingEnergy)
	: m_worldName(name), m_worldEra(1), m_worldEnergy(startingEnergy), m_tickCount(0) {
}

void World_Creation::AdvanceTime(int ticks) {
	for (int t = 0; t < ticks; ++t) {
		m_tickCount++;
		unsigned long long tickEnergy = 0;

		for (size_t i = 0; i < m_biomes.size(); ++i) {
			m_biomes[i].ProcessTick();
			tickEnergy += m_biomes[i].CalculateEnergyYield();
		}
		m_worldEnergy += tickEnergy;
	}
	std::cout << "\n>> Time Advanced by " << ticks << "epoch(s). Total Energy: " << m_worldEnergy << "\n";
}

bool World_Creation::CreateBiome(const std::string& name, unsigned long long habitability, unsigned long long cost) {
	if (m_worldEnergy < cost) {
		std::cout << "\n[!] Not Enough World Energy! Need " << cost << ".\n";
		return false;
	}
	m_worldEnergy -= cost;
	m_biomes.push_back(Biome(name, habitability));
	std::cout << "\n[+] Manifested new biome: " << name << "!\n";
	return true;
}

bool World_Creation::UpgradeBiome(int index) {
	if (index < 0 || index >= static cast<int>(m_biomes.size())) {
		std::cout << "\n[!] Invalid biome selection.\n";
		return false;
	}
	m_worldEnergy -= cost;
	m_biomes[index].UpgradeTier();
	std::cout << "\n[+] Upgraded " << m_biomes[index].GetName() << " to Tier " << m_biomes[index].GetTier() << "!\n";
	return true;
}

void World_Creation::DisplayStatus() const {
	std::cout << "\n============================\n";
	std::cout << " WORLD: " << m_worldName << " | EPOCH: " << m_tickCount << "\n";
	std::cout << " WORLD ENERGY: " << m_worldEnergy << "\n";
	std::cout << "==============================\n";

	if (m_biomes.empty()) {
		std::cout << " No biomes manifested yet. The world is barren.\n";
	}
	else {
		for (size_t i = 0; i < m_biomes.size(); ++i) {
			std::cout << " [" << i << "] " << m_biomes[i].GetName()
				<< " | Tier: " << m_biomes[i].GetTier()
				<< " | Pop: " << m_biomes[i].GetPopulation() << "/" << m_biomes[i].GetPopulationCap()
				<< " | Yield: +" << m_biomes[i].CalculateEnergyYield() << "/tick"
				<< " | Upgrade Cost: " << m_biomes[i].GetUpgradeCost() << "\n";
		}
	}
	std::cout << "====================================\n";
}