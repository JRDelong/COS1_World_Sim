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
	std::cout << "\n>> Time Advanced by " << ticks << " epoch(s). Total Energy: " << m_worldEnergy << "\n";
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
	if (m_worldEnergy < cost) {
		std::cout << "\n[!] Insufficient energy. Upgrade requires " << cost << " Energy.\n";
		return false;
	}

	m_worldEnergy -= cost;
	m_biomes[index].UpgradeTier();
	std::cout << "\n[+] Upgraded " << m_biomes[index].GetName() << " to Tier " << m_biomes[index].GetTier() << "!\n";
	return true;
}

bool World_Creation::SeedSentientRace(int biomeIndex, const std::string& raceName, const std::string& trait, unsigned long long cost) {
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
	if (m_worldEnergy < cost) {
		std::cout << "\n[!] Not enough World Energy! Awakening sentient life requires " << cost << " Energy.\n";
		return false;
	}

	m_worldEnergy -= cost;
	m_biomes[biomeIndex].SeedRace(raceName, trait);
	std::cout << "\n[+] Sparked Sentient Life! The " << raceName << " (" << trait << ") have awakened in "
		<< m_biomes[biomeIndex].GetName() << "!\n";
	return true;
}

bool World_Creation::CreateBiome(const Biome& newBiome, unsigned long long cost) {
	if (m_worldEnergy < cost) {
		std::cout << "\n[!] Not enough World Energy! Need " << cost << ".\n";
		return false;
	}
	m_worldEnergy -= cost;
	m_biomes.push_back(newBiome);
	std::cout << "\n[+] Manifested " << newBiome.GetName() << " [" << newBiome.GetTrait() << "]!\n";
	return true;
}

void World_Creation::DisplayStatus() const {
	std::cout << "\n======================================================\n";
	std::cout << " WORLD: " << m_worldName << " | EPOCH: " << m_tickCount << "\n";
	std::cout << " WORLD ENERGY: " << m_worldEnergy << "\n";
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
				<< " | Yield: +" << m_biomes[i].CalculateEnergyYield() << "/tick"
				<< " | Upgrade Cost: " << m_biomes[i].GetUpgradeCost() << "\n"
				<< "     Sentient Life: " << m_biomes[i].GetRaceSummary() << "\n";
		}
	}
	std::cout << "======================================================\n";
}

