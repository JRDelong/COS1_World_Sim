#include "Sim.h"
#include <algorithm>

Sim::Sim()
    : currentTick(0),
    overseerPoints(0),
    totalExpansions(0),
    totalTrades(0),
    totalRaids(0),
    totalAdaptations(0) {
}

void Sim::AdvanceTicks(int tickCount) {
    if (tickCount <= 0) {
        return;
    }
    for (int i = 0; i < tickCount; ++i) {
        StepSingleTick();
    }
}

void Sim::StepSingleTick() {
    ++currentTick;

    UpdateModifiers();
    UpdateResources();

    int aggressionDrift = GetTotalAggressionDriftMod();

    for (size_t i = 0; i < agents.size(); ++i) {
        if (agents[i].population <= 0) {
            continue;
        }

        if (aggressionDrift != 0) {
            agents[i].aggression = ClampValue(agents[i].aggression + aggressionDrift, 0, 100);
        }

        EvaluateAgentNeeds(agents[i]);
        ExecuteAgentAction(agents[i]);
        UpdatePopulationAndTraits(agents[i]);
    }

    ProcessAchievements();
}

void Sim::AddSector(const std::string& name, BiomeType biome, int food, int materials) {
    Sector newSector;
    newSector.id = static_cast<int>(sectors.size());
    newSector.name = name;
    newSector.biome = biome;
    newSector.foodSupply = ClampValue(food, 0, 500);
    newSector.materialSupply = ClampValue(materials, 0, 500);
    newSector.ownerAgentId = -1;
    sectors.push_back(newSector);

    LogEvent(EventCategory::Nature, "A new region formed in the world: " + name + ".");
}

void Sim::AddAgent(const std::string& name, const std::string& species, BiomeType preferredBiome, int startSectorId) {
    if (startSectorId < 0 || startSectorId >= static_cast<int>(sectors.size())) {
        return;
    }
    if (sectors[startSectorId].ownerAgentId != -1) {
        return;
    }

    Agent newAgent;
    newAgent.id = static_cast<int>(agents.size());
    newAgent.name = name;
    newAgent.species = species;
    newAgent.preferredBiome = preferredBiome;
    newAgent.population = 20;
    newAgent.food = 40;
    newAgent.materials = 20;
    newAgent.aggression = 30;
    newAgent.adaptability = 50;
    newAgent.homeSectorId = startSectorId;
    newAgent.currentGoal = AgentGoal::Forage;
    newAgent.targetSectorId = startSectorId;

    sectors[startSectorId].ownerAgentId = newAgent.id;
    agents.push_back(newAgent);

    LogEvent(EventCategory::Civilization, newAgent.name + " (" + newAgent.species + ") arose in " + sectors[startSectorId].name + ".");
    ProcessAchievements();
}

void Sim::ApplyWorldModifier(const WorldModifier& modifier) {
    for (size_t i = 0; i < activeModifiers.size(); ++i) {
        if (activeModifiers[i].id == modifier.id) {
            if (!modifier.isPermanent) {
                activeModifiers[i].remainingTicks = modifier.remainingTicks;
                LogEvent(EventCategory::WorldLaw, "World ability renewed: " + modifier.name + ".");
            }
            return;
        }
    }

    activeModifiers.push_back(modifier);

    if (modifier.isPermanent) {
        LogEvent(EventCategory::WorldLaw, "A new World Law was etched into reality: " + modifier.name + ".");
    }
    else {
        LogEvent(EventCategory::WorldLaw, "World phenomenon began: " + modifier.name + " (" + std::to_string(modifier.remainingTicks) + " ticks).");
    }
}

bool Sim::HasActiveModifier(const std::string& modifierId) const {
    for (size_t i = 0; i < activeModifiers.size(); ++i) {
        if (activeModifiers[i].id == modifierId) {
            return true;
        }
    }
    return false;
}

int Sim::GetOverseerPoints() const {
    return overseerPoints;
}

bool Sim::SpendOverseerPoints(int amount) {
    if (amount < 0 || overseerPoints < amount) {
        return false;
    }
    overseerPoints -= amount;
    return true;
}

int Sim::GetCurrentTick() const {
    return currentTick;
}

const std::vector<Sector>& Sim::GetSectors() const {
    return sectors;
}

const std::vector<Agent>& Sim::GetAgents() const {
    return agents;
}

const std::vector<WorldModifier>& Sim::GetActiveModifiers() const {
    return activeModifiers;
}

const Achievement& Sim::GetAchievement() const {
    return achievement;
}

std::vector<SimEvent> Sim::GetRecentEvents(int maxCount) const {
    if (maxCount <= 0 || eventLog.empty()) {
        return std::vector<SimEvent>();
    }
    int total = static_cast<int>(eventLog.size());
    int startIdx = (total > maxCount) ? (total - maxCount) : 0;

    std::vector<SimEvent> recent;
    for (int i = startIdx; i < total; ++i) {
        recent.push_back(eventLog[i]);
    }
    return recent;
}

void Sim::UpdateModifiers() {
    for (size_t i = 0; i < activeModifiers.size(); ) {
        if (!activeModifiers[i].isPermanent) {
            --activeModifiers[i].remainingTicks;
            if (activeModifiers[i].remainingTicks <= 0) {
                LogEvent(EventCategory::WorldLaw, "World phenomenon faded: " + activeModifiers[i].name + ".");
                activeModifiers.erase(activeModifiers.begin() + i);
                continue;
            }
        }
        ++i;
    }
}

void Sim::UpdateResources() {
    int globalFoodMod = GetTotalFoodRegenMod();
    int globalMatMod = GetTotalMaterialRegenMod();

    for (size_t i = 0; i < sectors.size(); ++i) {
        int foodRegen = 4;
        int matRegen = 2;

        if (sectors[i].biome == BiomeType::Forest || sectors[i].biome == BiomeType::Plains) {
            foodRegen = 7;
        }
        else if (sectors[i].biome == BiomeType::Mountains) {
            foodRegen = 2;
            matRegen = 6;
        }
        else if (sectors[i].biome == BiomeType::Desert) {
            foodRegen = 1;
            matRegen = 3;
        }

        int finalFoodRegen = std::max(0, foodRegen + globalFoodMod);
        int finalMatRegen = std::max(0, matRegen + globalMatMod);

        sectors[i].foodSupply = ClampValue(sectors[i].foodSupply + finalFoodRegen, 0, 500);
        sectors[i].materialSupply = ClampValue(sectors[i].materialSupply + finalMatRegen, 0, 500);
    }
}

void Sim::EvaluateAgentNeeds(Agent& agent) {
    if (sectors.empty()) {
        return;
    }

    int foodNeeded = agent.population * 2;
    agent.targetSectorId = agent.homeSectorId;

    // 1. Urgent survival: low food triggers foraging across owned sectors
    if (agent.food < foodNeeded) {
        agent.currentGoal = AgentGoal::Forage;
        int bestFood = -1;
        for (size_t i = 0; i < sectors.size(); ++i) {
            if (sectors[i].ownerAgentId == agent.id && sectors[i].foodSupply > bestFood) {
                bestFood = sectors[i].foodSupply;
                agent.targetSectorId = sectors[i].id;
            }
        }
        return;
    }

    // 2. Hostile expansion: high aggression + strong population triggers raiding
    if (agent.aggression >= 65 && agent.population >= 25) {
        for (size_t i = 0; i < sectors.size(); ++i) {
            if (sectors[i].ownerAgentId != -1 && sectors[i].ownerAgentId != agent.id) {
                agent.currentGoal = AgentGoal::Raid;
                agent.targetSectorId = sectors[i].id;
                return;
            }
        }
    }

    // 3. Peaceful expansion: surplus food & materials allow claiming an open sector
    if (agent.food > foodNeeded + 30 && agent.materials >= 25) {
        int bestScore = -1000;
        int bestSector = -1;

        for (size_t i = 0; i < sectors.size(); ++i) {
            if (sectors[i].ownerAgentId == -1) {
                int score = CalculateBiomeHarmony(agent, sectors[i]) + sectors[i].foodSupply;
                if (score > bestScore) {
                    bestScore = score;
                    bestSector = sectors[i].id;
                }
            }
        }

        if (bestSector != -1) {
            agent.currentGoal = AgentGoal::Expand;
            agent.targetSectorId = bestSector;
            return;
        }
    }

    // 4. Trade if materials are scarce but food is abundant
    if (agent.materials < 15 && agent.food > foodNeeded + 20) {
        for (size_t i = 0; i < agents.size(); ++i) {
            if (agents[i].id != agent.id && agents[i].population > 0 && agents[i].aggression < 60 && agents[i].materials >= 20) {
                agent.currentGoal = AgentGoal::Trade;
                agent.targetSectorId = agents[i].homeSectorId;
                return;
            }
        }
    }

    // 5. Default: Fortify and gather materials at home
    agent.currentGoal = AgentGoal::Fortify;
    agent.targetSectorId = agent.homeSectorId;
}

void Sim::ExecuteAgentAction(Agent& agent) {
    if (agent.targetSectorId < 0 || agent.targetSectorId >= static_cast<int>(sectors.size())) {
        return;
    }

    Sector& target = sectors[agent.targetSectorId];
    int harmony = CalculateBiomeHarmony(agent, target);

    switch (agent.currentGoal) {
    case AgentGoal::Forage: {
        int baseGather = 10 + (agent.population / 4) + (harmony / 10);
        if (baseGather < 4) {
            baseGather = 4;
        }
        int gathered = std::min(target.foodSupply, baseGather);
        target.foodSupply -= gathered;
        agent.food += gathered;
        break;
    }

    case AgentGoal::Expand: {
        if (target.ownerAgentId == -1 && agent.materials >= 20) {
            agent.materials -= 20;
            target.ownerAgentId = agent.id;
            ++totalExpansions;
            LogEvent(EventCategory::Civilization, agent.name + " expanded into " + target.name + ".");
        }
        break;
    }

    case AgentGoal::Trade: {
        int partnerId = target.ownerAgentId;
        if (partnerId >= 0 && partnerId < static_cast<int>(agents.size()) && partnerId != agent.id) {
            Agent& partner = agents[partnerId];
            if (agent.food >= 15 && partner.materials >= 10) {
                int bonus = std::max(0, GetTotalTradeBonusMod());
                agent.food -= 15;
                partner.food += 15 + bonus;
                partner.materials -= 10;
                agent.materials += 10 + bonus;
                ++totalTrades;
                LogEvent(EventCategory::Civilization, agent.name + " traded food for materials with " + partner.name + ".");
            }
        }
        break;
    }

    case AgentGoal::Fortify: {
        int gatheredMat = std::min(target.materialSupply, 6 + (agent.population / 8));
        target.materialSupply -= gatheredMat;
        agent.materials += gatheredMat;
        break;
    }

    case AgentGoal::Raid: {
        int defenderId = target.ownerAgentId;
        if (defenderId >= 0 && defenderId < static_cast<int>(agents.size()) && defenderId != agent.id) {
            Agent& defender = agents[defenderId];
            ++totalRaids;

            int attackPower = agent.population + (agent.aggression / 5);
            int defensePower = defender.population + (defender.materials / 4);

            if (attackPower > defensePower) {
                int stolenFood = std::min(defender.food, 20);
                defender.food -= stolenFood;
                agent.food += stolenFood;
                defender.population = ClampValue(defender.population - 3, 0, 10000);
                agent.population = ClampValue(agent.population - 1, 0, 10000);

                if (defender.population == 0 || target.id != defender.homeSectorId) {
                    target.ownerAgentId = agent.id;
                    LogEvent(EventCategory::Conflict, agent.name + " raided and seized " + target.name + " from " + defender.name + "!");
                }
                else {
                    LogEvent(EventCategory::Conflict, agent.name + " raided " + defender.name + " in " + target.name + " for supplies!");
                }
            }
            else {
                agent.population = ClampValue(agent.population - 3, 0, 10000);
                agent.aggression = ClampValue(agent.aggression - 10, 0, 100);
                LogEvent(EventCategory::Conflict, defender.name + " repelled a raid by " + agent.name + " at " + target.name + ".");
            }
        }
        break;
    }
    }
}

void Sim::UpdatePopulationAndTraits(Agent& agent) {
    int consumption = std::max(1, agent.population / 2);

    if (agent.food >= consumption) {
        agent.food -= consumption;

        if (agent.food > agent.population * 2) {
            agent.population += 2;
        }
        else if (agent.food > agent.population) {
            agent.population += 1;
        }
    }
    else {
        int deficit = consumption - agent.food;
        agent.food = 0;
        agent.population = ClampValue(agent.population - std::max(1, deficit), 0, 10000);
        agent.aggression = ClampValue(agent.aggression + 4, 0, 100);

        if (agent.population == 0) {
            LogEvent(EventCategory::Civilization, agent.name + " (" + agent.species + ") perished from famine.");
            for (size_t i = 0; i < sectors.size(); ++i) {
                if (sectors[i].ownerAgentId == agent.id) {
                    sectors[i].ownerAgentId = -1;
                }
            }
            return;
        }
    }

    if (agent.homeSectorId >= 0 && agent.homeSectorId < static_cast<int>(sectors.size())) {
        BiomeType homeBiome = sectors[agent.homeSectorId].biome;
        if (homeBiome != agent.preferredBiome) {
            agent.adaptability = ClampValue(agent.adaptability + 1, 0, 100);
            if (agent.adaptability >= 100) {
                agent.preferredBiome = homeBiome;
                ++totalAdaptations;
                LogEvent(EventCategory::Nature, agent.name + " adapted to thrive in " + sectors[agent.homeSectorId].name + ".");
            }
        }
    }
}

void Sim::ProcessAchievements() {
    WorldMilestoneStats stats;
    stats.totalSectors = static_cast<int>(sectors.size());
    stats.totalExpansions = totalExpansions;
    stats.totalTrades = totalTrades;
    stats.totalRaids = totalRaids;
    stats.totalAdaptations = totalAdaptations;

    for (size_t i = 0; i < agents.size(); ++i) {
        if (agents[i].population > 0) {
            ++stats.activeAgents;
            if (agents[i].population > stats.maxPopulation) {
                stats.maxPopulation = agents[i].population;
            }
        }
    }

    std::vector<AchievementItem> unlockedNow = achievement.Evaluate(stats);
    for (size_t i = 0; i < unlockedNow.size(); ++i) {
        overseerPoints += unlockedNow[i].rewardPoints;
        LogEvent(EventCategory::Achievement,
            unlockedNow[i].title + " - " + unlockedNow[i].description +
            " (+" + std::to_string(unlockedNow[i].rewardPoints) + " OP)");
    }
}

int Sim::GetTotalFoodRegenMod() const {
    int total = 0;
    for (size_t i = 0; i < activeModifiers.size(); ++i) {
        total += activeModifiers[i].foodRegenDelta;
    }
    return total;
}

int Sim::GetTotalMaterialRegenMod() const {
    int total = 0;
    for (size_t i = 0; i < activeModifiers.size(); ++i) {
        total += activeModifiers[i].materialRegenDelta;
    }
    return total;
}

int Sim::GetTotalAggressionDriftMod() const {
    int total = 0;
    for (size_t i = 0; i < activeModifiers.size(); ++i) {
        total += activeModifiers[i].aggressionDriftDelta;
    }
    return total;
}

int Sim::GetTotalTradeBonusMod() const {
    int total = 0;
    for (size_t i = 0; i < activeModifiers.size(); ++i) {
        total += activeModifiers[i].tradeBonusDelta;
    }
    return total;
}

int Sim::GetTotalHarmonyMod() const {
    int total = 0;
    for (size_t i = 0; i < activeModifiers.size(); ++i) {
        total += activeModifiers[i].harmonyBonusDelta;
    }
    return total;
}

int Sim::CalculateBiomeHarmony(const Agent& agent, const Sector& sector) const {
    int baseHarmony = (agent.preferredBiome == sector.biome) ? 30 : (-20 + (agent.adaptability / 5));
    return baseHarmony + GetTotalHarmonyMod();
}

void Sim::LogEvent(EventCategory category, const std::string& text) {
    SimEvent entry;
    entry.tick = currentTick;
    entry.category = category;
    entry.message = text;
    eventLog.push_back(entry);

    if (eventLog.size() > 200) {
        eventLog.erase(eventLog.begin());
    }
}

int Sim::ClampValue(int value, int minVal, int maxVal) const {
    if (value < minVal) {
        return minVal;
    }
    if (value > maxVal) {
        return maxVal;
    }
    return value;
}




