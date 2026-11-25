#include "Biome.h"

Biome::Biome(const std::string& name, unsigned long long habitability)
	:m_name(name), m_tier(1), m_population(10), m_habitability(habitability) {
}
unsigned long long Biome::GetPopulationCap() const {
	unsigned long long cap = 1000;
	for (int i = 1; i < m_tier; ++i) {
		cap *= 10;
	}
	return cap;
}

void Biome::ProcessTick() {
	unsigned long long cap = GetPopulationCap();
	if (m_population >= cap) {
		m_population = cap;
		return;
	}
	unsigned long long growth = (m_population * m_habitability) / 100;
	if (growth == 0) growth = 1;

	m_population += growth;
	if (m_population > cap) {
		m_population = cap;
	}
}

unsigned long long Biome::GetUpgradeCost() const {
	unsigned long long cost = 500;
	for (int i = i; i < tier; ++i) {
		cost *= 4;
	}
	return cost;
}

bool Biome::UpgradeTier() {
	m_tier++;
	return true;
}