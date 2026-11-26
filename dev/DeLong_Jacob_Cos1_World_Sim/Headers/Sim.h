#ifndef SIM_H
#define SIM_H

#include "Achievement.h"
#include <string>
#include <vector>

enum class BiomeType {
    Forest,
    Plains,
    Mountains,
    Desert,
    Wetlands
};

enum class AgentGoal {
    Forage,
    Expand,
    Trade,
    Fortify,
    Raid
};

enum class EventCategory {
    Civilization,
    Conflict,
    Nature,
    WorldLaw,
    Achievement
};

struct Sector {
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

    int population = 20;
    int food = 40;
    int materials = 20;
    int aggression = 30;
    int adaptability = 50;
    int homeSectorId = 0;

    AgentGoal currentGoal = AgentGoal::Forage;
    int targetSectorId = -1;
};

struct WorldModifier {
    std::string id;
    std::string name;
    std::string description;
    bool isPermanent = true;
    int remainingTicks = -1;

    int foodRegenDelta = 0;
    int materialRegenDelta = 0;
    int aggressionDriftDelta = 0;
    int tradeBonusDelta = 0;
    int harmonyBonusDelta = 0;
};

struct SimEvent {
    int tick = 0;
    EventCategory category = EventCategory::Civilization;
    std::string message;
};

class Sim {
public:
    Sim();

    void AdvanceTicks(int tickCount);
    void StepSingleTick();

    void AddSector(const std::string& name, BiomeType biome, int food, int materials);
    void AddAgent(const std::string& name, const std::string& species, BiomeType preferredBiome, int startSectorId);

    void ApplyWorldModifier(const WorldModifier& modifier);
    bool HasActiveModifier(const std::string& modifierId) const;

    int GetOverseerPoints() const;
    bool SpendOverseerPoints(int amount);

    int GetCurrentTick() const;
    const std::vector<Sector>& GetSectors() const;
    const std::vector<Agent>& GetAgents() const;
    const std::vector<WorldModifier>& GetActiveModifiers() const;
    const Achievement& GetAchievement() const;
    std::vector<SimEvent> GetRecentEvents(int maxCount) const;

private:
    int currentTick = 0;
    int overseerPoints = 0;

    int totalExpansions = 0;
    int totalTrades = 0;
    int totalRaids = 0;
    int totalAdaptations = 0;

    std::vector<Sector> sectors;
    std::vector<Agent> agents;
    std::vector<WorldModifier> activeModifiers;
    std::vector<SimEvent> eventLog;
    Achievement achievement;

    void UpdateModifiers();
    void UpdateResources();
    void EvaluateAgentNeeds(Agent& agent);
    void ExecuteAgentAction(Agent& agent);
    void UpdatePopulationAndTraits(Agent& agent);
    void ProcessAchievements();

    int GetTotalFoodRegenMod() const;
    int GetTotalMaterialRegenMod() const;
    int GetTotalAggressionDriftMod() const;
    int GetTotalTradeBonusMod() const;
    int GetTotalHarmonyMod() const;

    int CalculateBiomeHarmony(const Agent& agent, const Sector& sector) const;
    void LogEvent(EventCategory category, const std::string& text);
    int ClampValue(int value, int minVal, int maxVal) const;
};

#endif 


