#ifndef SIM_H
#define SIM_H
#pragma once
#include <string>
#include <vector>

enum BiomeType {
	Forest,
	Plains,
	Mountains,
	Desert,
	Wetlands
};

enum class AgentGoal
{
	Forage,
	Expand,
	Trade,
	Fortify,
	Raid
};

struct Sector 
{		
	int id = 0;
	std::string name;
	BiomeType biome = BiomeType::Plains;
	int foodSupply = 50;
	int materialSupply = 50;
	int ownerAgentId = -1;
};
struct Agent {
	int id = 0;
	std::string name;
	std::string species;
	BiomeType preferredBiome = BiomeType::Plains;

	AgentGoal currentGoal = AgentGoal::Forage;
	int targetSectorId = -1;
};
struct SimEvent {
	int tick = 0;
	std::string message;
};
struct SimEvent {
	int tick = 0;
	std::string message;
};

class Sim
{
public:
	Sim();
	void AdvancedTicks(int tickCount);
	void StepSingleTick();
	void AddSector(const std::string& name, BiomeType biome, int food, int materials);
	void AddAgent(const std::string& name, const std::string& species, BiomeType preferredBiome, int startSectorId);
	bool BlessSector(int agentId, int delta);

	int GetCurrentTick() const;
	int GetOverseerEnergy() const;
	const std::vector<Sector>& GetSectors() const;
	const std::vector<Agent>& GetAgents() const;
	std::vector<SimEvent> GetRecentEvents(int maxCount)const;

private:
	int currentTick = 0;
	int OverseerEnergy = 100;
	int maxOverseerEnergy = 100;

	std::vector<Sector> sectors;
	std::vector<Agent> agents;
	std::vector<SimEvent> eventLog;

	void UpdateResources();
	void EvaluateAgentNeeds(Agent& agent);
	void ExecuteAgentAction(Agent& agent);
	void UpdatePopulationAndTraits(Agent& agent);

	int CalculateBiomeHarmony(const Agent& agent, const Sector& sector)const;
	void LogEvent(const std::string& text);
	int ClampValue(int value, int minVal, int maxVal)const;

};

#endif