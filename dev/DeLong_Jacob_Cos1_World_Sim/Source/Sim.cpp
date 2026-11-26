#include "Sim.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>

Sim::Sim()
    : currentTick(0),
    totalExpansions(0),
    totalTrades(0),
    totalRaids(0),
    totalAdaptations(0) {
}
bool Sim::SuggestAgentGoal(int agentId, AgentGoal goal) {
    if (agentId < 0 || agentId >= static_cast<int>(agents.size())) {
        return false;
    }
    if (agents[agentId].population <= 0) {
        return false;
    }

    agents[agentId].hasGuidance = true;
    agents[agentId].suggestedGoal = goal;
    LogEvent(EventCategory::WorldLaw,
        "The Overseer whispered guidance to " + agents[agentId].name + " (" + GoalToString(goal) + ").");
    return true;
}

int Sim::ComputeDeterministicRoll(int salt) const {
    unsigned int seed = static_cast<unsigned int>((currentTick + 1) * 1103515245u + (salt + 7) * 12345u);
    return static_cast<int>((seed >> 16) % 100u);
}

bool Sim::TryApplyGuidance(Agent& agent) {
    if (!agent.hasGuidance) {
        return false;
    }

    AgentGoal whisper = agent.suggestedGoal;
    agent.hasGuidance = false; // Consume the whisper this tick

    int consumption = std::max(1, agent.population / 3);
    int emergencyFood = consumption * 2;

    // 1. Survival instinct overrides non-food whispers during famine
    if (agent.food < emergencyFood && whisper != AgentGoal::Forage && whisper != AgentGoal::Farm) {
        LogEvent(EventCategory::Civilization,
            agent.name + " ignored the Overseer's guidance to " + GoalToString(whisper) + " due to hunger.");
        return false;
    }

    // 2. Calculate compliance chance based on personality & pride
    int complianceChance = 55;

    if (whisper == AgentGoal::Raid) {
        if (agent.aggression >= 55) {
            complianceChance += 25;
        }
        else if (agent.aggression < 35) {
            complianceChance -= 30;
        }
    }
    else if (whisper == AgentGoal::Trade || whisper == AgentGoal::Farm) {
        if (agent.aggression < 45) {
            complianceChance += 20;
        }
        else if (agent.aggression >= 70) {
            complianceChance -= 20;
        }
    }
    else if (whisper == AgentGoal::Expand) {
        if (agent.materials >= 15) {
            complianceChance += 15;
        }
        else {
            complianceChance -= 25;
        }
    }

    if (agent.population >= 60) {
        complianceChance -= 15; // Large empires are prouder and more independent
    }

    complianceChance = ClampValue(complianceChance, 10, 90);
    int roll = ComputeDeterministicRoll(agent.id * 31 + agent.population);

    if (roll >= complianceChance) {
        LogEvent(EventCategory::Civilization,
            agent.name + " ignored the Overseer's guidance to " + GoalToString(whisper) + " and followed its own instincts.");
        return false;
    }

    // 3. Agent heeds the whisper — pick an appropriate target sector
    agent.currentGoal = whisper;
    agent.targetSectorId = agent.homeSectorId;

    if (whisper == AgentGoal::Forage) {
        int bestFood = -1;
        for (size_t i = 0; i < sectors.size(); ++i) {
            if (sectors[i].ownerAgentId == agent.id && sectors[i].foodSupply > bestFood) {
                bestFood = sectors[i].foodSupply;
                agent.targetSectorId = sectors[i].id;
            }
        }
    }
    else if (whisper == AgentGoal::Farm) {
        int bestRegen = -1;
        for (size_t i = 0; i < sectors.size(); ++i) {
            if (sectors[i].ownerAgentId == agent.id) {
                int regen = Biome::GetBaseFoodRegen(sectors[i].biome);
                if (regen > bestRegen) {
                    bestRegen = regen;
                    agent.targetSectorId = sectors[i].id;
                }
            }
        }
    }
    else if (whisper == AgentGoal::Expand) {
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
        if (bestSector == -1) {
            LogEvent(EventCategory::Civilization,
                agent.name + " wished to heed the Overseer, but found no unclaimed land to Expand into.");
            return false;
        }
        agent.targetSectorId = bestSector;
    }
    else if (whisper == AgentGoal::Trade) {
        int partnerSector = -1;
        for (size_t i = 0; i < agents.size(); ++i) {
            if (agents[i].id != agent.id && agents[i].population > 0) {
                partnerSector = agents[i].homeSectorId;
                break;
            }
        }
        if (partnerSector == -1) {
            LogEvent(EventCategory::Civilization,
                agent.name + " wished to Trade, but no other civilizations exist.");
            return false;
        }
        agent.targetSectorId = partnerSector;
    }
    else if (whisper == AgentGoal::Raid) {
        int rivalSector = -1;
        for (size_t i = 0; i < sectors.size(); ++i) {
            if (sectors[i].ownerAgentId != -1 && sectors[i].ownerAgentId != agent.id) {
                rivalSector = sectors[i].id;
                break;
            }
        }
        if (rivalSector == -1) {
            LogEvent(EventCategory::Civilization,
                agent.name + " looked for a rival to Raid, but found none.");
            return false;
        }
        agent.targetSectorId = rivalSector;
    }
    else if (whisper == AgentGoal::Fortify) {
        int bestMat = -1;
        for (size_t i = 0; i < sectors.size(); ++i) {
            if (sectors[i].ownerAgentId == agent.id && sectors[i].materialSupply > bestMat) {
                bestMat = sectors[i].materialSupply;
                agent.targetSectorId = sectors[i].id;
            }
        }
    }

    LogEvent(EventCategory::Civilization,
        agent.name + " heeded the Overseer's guidance to " + GoalToString(whisper) + "!");
    return true;
}

void Sim::RenderGuidanceMenu() {
    ClearConsole();
    std::cout << "=================================================================\n";
    std::cout << "  WORLD OVERSEER - DIVINE GUIDANCE (Indirect Whisper)\n";
    std::cout << "=================================================================\n";
    std::cout << "  Whisper a suggested action to a civilization. They may heed\n";
    std::cout << "  your guidance or ignore it based on hunger, pride, and nature.\n\n";

    std::cout << "  [ACTIVE CIVILIZATIONS]\n";
    for (size_t i = 0; i < agents.size(); ++i) {
        if (agents[i].population > 0) {
            std::cout << "    [" << agents[i].id << "] " << agents[i].name
                << " (" << agents[i].species << " | Aggr: " << agents[i].aggression
                << " | Pop: " << agents[i].population << ")\n";
        }
    }

    std::cout << "\n  Enter Civilization ID (or [Q] to cancel): ";
    std::string idInput;
    if (!std::getline(std::cin, idInput) || idInput == "q" || idInput == "Q" || idInput.empty()) {
        return;
    }

    char* endPtr = nullptr;
    long chosenId = std::strtol(idInput.c_str(), &endPtr, 10);
    if (endPtr == idInput.c_str() || *endPtr != '\0' ||
        chosenId < 0 || chosenId >= static_cast<long>(agents.size()) ||
        agents[chosenId].population <= 0) {
        PauseForEnter("Invalid civilization ID. Press Enter to return...");
        return;
    }

    std::cout << "\n  Choose a suggested goal for " << agents[chosenId].name << ":\n";
    std::cout << "    [1] Farm (Cultivate crops)\n";
    std::cout << "    [2] Forage (Gather wild food)\n";
    std::cout << "    [3] Expand (Claim open land)\n";
    std::cout << "    [4] Trade (Exchange goods with neighbors)\n";
    std::cout << "    [5] Fortify (Stockpile materials)\n";
    std::cout << "    [6] Raid (Attack a rival sector)\n";
    std::cout << "  Select goal (1-6): ";

    std::string goalInput;
    if (!std::getline(std::cin, goalInput)) {
        return;
    }

    AgentGoal chosenGoal = AgentGoal::Farm;
    if (goalInput == "1") {
        chosenGoal = AgentGoal::Farm;
    }
    else if (goalInput == "2") {
        chosenGoal = AgentGoal::Forage;
    }
    else if (goalInput == "3") {
        chosenGoal = AgentGoal::Expand;
    }
    else if (goalInput == "4") {
        chosenGoal = AgentGoal::Trade;
    }
    else if (goalInput == "5") {
        chosenGoal = AgentGoal::Fortify;
    }
    else if (goalInput == "6") {
        chosenGoal = AgentGoal::Raid;
    }
    else {
        PauseForEnter("Invalid goal selection. Press Enter to return...");
        return;
    }

    SuggestAgentGoal(static_cast<int>(chosenId), chosenGoal);
    PauseForEnter("Whisper sent! Advance a tick to see if they heed or ignore it...");
}

void Sim::RunObserverSession(Player& overseer, const GameSettings& settings) {
    ProcessAchievements(&overseer);

    bool running = true;
    while (running) {
        RenderHUD(overseer, settings);

        std::string input;
        if (!std::getline(std::cin, input)) {
            break;
        }

        if (input.empty()) {
            StepSingleTick(&overseer);
        }
        else if (input == "f" || input == "F") {
            AdvanceTicks(settings.fastForwardStep, &overseer);
        }
        else if (input == "a" || input == "A") {
            RenderAchievementsScreen(overseer);
        }
        else if (input == "q" || input == "Q") {
            running = false;
        }
        else if (input == "g" || input == "G") {
            RenderGuidanceMenu();
        }

        else 
        {
            char* endPtr = nullptr;
            long ticks = std::strtol(input.c_str(), &endPtr, 10);
            if (endPtr != input.c_str() && *endPtr == '\0' && ticks > 0) 
            {
                if (ticks > 500) 
                {
                    ticks = 500;
                }
                AdvanceTicks(static_cast<int>(ticks), &overseer);
            }
        }
    }
}

void Sim::AdvanceTicks(int tickCount, Player* overseer) {
    if (tickCount <= 0) {
        return;
    }
    for (int i = 0; i < tickCount; ++i) {
        StepSingleTick(overseer);
    }
}

void Sim::StepSingleTick(Player* overseer) {
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

    ProcessAchievements(overseer);
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

void Sim::AddAgent(const std::string& name,
    const std::string& species,
    BiomeType preferredBiome,
    int startSectorId,
    int startingAggression,
    int startingAdaptability,
    Player* overseer) {
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
    newAgent.aggression = ClampValue(startingAggression, 0, 100);
    newAgent.adaptability = ClampValue(startingAdaptability, 0, 100);
    newAgent.homeSectorId = startSectorId;
    newAgent.currentGoal = AgentGoal::Forage;
    newAgent.targetSectorId = startSectorId;

    sectors[startSectorId].ownerAgentId = newAgent.id;
    agents.push_back(newAgent);

    LogEvent(EventCategory::Civilization,
        newAgent.name + " (" + newAgent.species + ") arose in " + sectors[startSectorId].name + ".");
    ProcessAchievements(overseer);
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
        LogEvent(EventCategory::WorldLaw,
            "World phenomenon began: " + modifier.name + " (" + std::to_string(modifier.remainingTicks) + " ticks).");
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

bool Sim::HasAnySector() const {
    return !sectors.empty();
}

bool Sim::HasAnyAgent() const {
    return !agents.empty();
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
        int foodRegen = Biome::GetBaseFoodRegen(sectors[i].biome);
        int matRegen = Biome::GetBaseMaterialRegen(sectors[i].biome);

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
    if (TryApplyGuidance(agent)) {
        return;
    }


    int consumption = std::max(1, agent.population / 3);
    int emergencyFood = consumption * 2;
    int safeFoodBuffer = consumption + 15;
    agent.targetSectorId = agent.homeSectorId;

    if (agent.food < emergencyFood) {
        int bestFood = 14;
        int bestSector = -1;
        for (size_t i = 0; i < sectors.size(); ++i) {
            if (sectors[i].ownerAgentId == agent.id && sectors[i].foodSupply > bestFood) {
                bestFood = sectors[i].foodSupply;
                bestSector = sectors[i].id;
            }
        }
        if (bestSector != -1) {
            agent.currentGoal = AgentGoal::Forage;
            agent.targetSectorId = bestSector;
            return;
        }
    }

    if (agent.food < safeFoodBuffer) {
        agent.currentGoal = AgentGoal::Farm;
        int bestRegen = -1;
        for (size_t i = 0; i < sectors.size(); ++i) {
            if (sectors[i].ownerAgentId == agent.id) {
                int regen = Biome::GetBaseFoodRegen(sectors[i].biome);
                if (regen > bestRegen) {
                    bestRegen = regen;
                    agent.targetSectorId = sectors[i].id;
                }
            }
        }
        return;
    }

    if (agent.materials >= 15) {
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

    if (agent.aggression >= 60 && agent.population >= 22) {
        for (size_t i = 0; i < sectors.size(); ++i) {
            if (sectors[i].ownerAgentId != -1 && sectors[i].ownerAgentId != agent.id) {
                agent.currentGoal = AgentGoal::Raid;
                agent.targetSectorId = sectors[i].id;
                return;
            }
        }
    }

    if (agent.materials < 25) {
        for (size_t i = 0; i < agents.size(); ++i) {
            if (agents[i].id != agent.id && agents[i].population > 0 && agents[i].aggression < 65 && agents[i].materials >= 15) {
                agent.currentGoal = AgentGoal::Trade;
                agent.targetSectorId = agents[i].homeSectorId;
                return;
            }
        }
    }

    agent.currentGoal = AgentGoal::Fortify;
    agent.targetSectorId = agent.homeSectorId;
    int bestMat = -1;
    for (size_t i = 0; i < sectors.size(); ++i) {
        if (sectors[i].ownerAgentId == agent.id && sectors[i].materialSupply > bestMat) {
            bestMat = sectors[i].materialSupply;
            agent.targetSectorId = sectors[i].id;
        }
    }
}




void Sim::ExecuteAgentAction(Agent& agent) {
    if (agent.targetSectorId < 0 || agent.targetSectorId >= static_cast<int>(sectors.size())) {
        return;
    }

    Sector& target = sectors[agent.targetSectorId];
    int harmony = CalculateBiomeHarmony(agent, target);

    switch (agent.currentGoal) {
    case AgentGoal::Forage: {
        int baseGather = 12 + (agent.population / 3) + (harmony / 10);
        if (baseGather < 8) {
            baseGather = 8;
        }
        int gathered = std::min(target.foodSupply, baseGather);
        target.foodSupply -= gathered;
        agent.food += gathered;

        if (gathered > 0) {
            LogEvent(EventCategory::Civilization,
                agent.name + " foraged " + std::to_string(gathered) + " wild food in " + target.name + ".");
        }
        break;
    }

    case AgentGoal::Farm: {
        int consumption = std::max(1, agent.population / 3);
        int biomeFertility = Biome::GetBaseFoodRegen(target.biome) * 2;
        int harvestedCrops = consumption + 6 + (biomeFertility / 2) + (harmony / 10);
        if (harvestedCrops < consumption + 5) {
            harvestedCrops = consumption + 5;
        }
        agent.food += harvestedCrops;

        if (currentTick % 3 == 0 || harvestedCrops >= 18) {
            LogEvent(EventCategory::Civilization,
                agent.name + " harvested " + std::to_string(harvestedCrops) + " crops in " + target.name + ".");
        }
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
    int consumption = std::max(1, agent.population / 3);

    if (agent.food >= consumption) {
        agent.food -= consumption;

        // Steady population growth whenever food reserves stay healthy
        if (agent.population < 200) {
            if (agent.food >= agent.population * 2) {
                agent.population += 2;
            }
            else if (agent.food >= consumption + 10) {
                agent.population += 1;
            }
        }

        // Growing civilizations naturally develop ambition/border tension over time
        if (agent.population >= 26 || agent.materials >= 35) {
            agent.aggression = ClampValue(agent.aggression + 2, 0, 100);
        }
    }
    else {
        int deficit = consumption - agent.food;
        agent.food = 0;
        agent.population = ClampValue(agent.population - std::max(1, deficit), 0, 10000);
        agent.aggression = ClampValue(agent.aggression + 5, 0, 100);

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
                LogEvent(EventCategory::Nature,
                    agent.name + " adapted to thrive in " + sectors[agent.homeSectorId].name + ".");
            }
        }
    }
}

void Sim::ProcessAchievements(Player* overseer) {
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
        if (overseer != nullptr) {
            overseer->EarnPoints(unlockedNow[i].rewardPoints);
        }
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

void Sim::RenderHUD(const Player& overseer, const GameSettings& settings) const {
    ClearConsole();
    std::cout << "=================================================================\n";
    std::cout << "  WORLD OVERSEER - OBSERVER CONSOLE\n";
    std::cout << "  Tick: " << currentTick
        << "  |  Overseer Points: " << overseer.GetOverseerPoints() << " OP"
        << "  |  Achievements: " << achievement.GetUnlockedCount() << "/" << achievement.GetTotalCount() << "\n";
    std::cout << "=================================================================\n";

    std::cout << "\n[ACTIVE WORLD LAWS & PHENOMENA]\n";
    if (activeModifiers.empty()) {
        std::cout << "  None enacted yet.\n";
    }
    else {
        for (size_t i = 0; i < activeModifiers.size(); ++i) {
            std::cout << "  - " << activeModifiers[i].name;
            if (!activeModifiers[i].isPermanent) {
                std::cout << " (" << activeModifiers[i].remainingTicks << " ticks left)";
            }
            else {
                std::cout << " (Permanent Law)";
            }
            std::cout << "\n";
        }
    }

    std::cout << "\n[WORLD SECTORS]\n";
    for (size_t i = 0; i < sectors.size(); ++i) {
        std::string ownerName = "Unclaimed";
        if (sectors[i].ownerAgentId >= 0 && sectors[i].ownerAgentId < static_cast<int>(agents.size())) {
            ownerName = agents[sectors[i].ownerAgentId].name;
        }
        std::cout << "  #" << sectors[i].id << " " << sectors[i].name
            << " (" << Biome::ToString(sectors[i].biome) << ")"
            << " | Owner: " << ownerName
            << " | Food: " << sectors[i].foodSupply
            << " | Mat: " << sectors[i].materialSupply << "\n";
    }

    std::cout << "\n[CIVILIZATIONS]\n";
    for (size_t i = 0; i < agents.size(); ++i) {
        if (agents[i].population <= 0) {
            std::cout << "  * " << agents[i].name << " (" << agents[i].species << ") - EXTINCT\n";
            continue;
        }
        std::string targetName = "None";
        if (agents[i].targetSectorId >= 0 && agents[i].targetSectorId < static_cast<int>(sectors.size())) {
            targetName = sectors[agents[i].targetSectorId].name;
        }
        std::cout << "  * [" << agents[i].id << "] " << agents[i].name << " (" << agents[i].species << ")"
            << " | Pop: " << agents[i].population
            << " | Food: " << agents[i].food
            << " | Mat: " << agents[i].materials
            << " | Aggr: " << agents[i].aggression
            << " | Goal: " << GoalToString(agents[i].currentGoal)
            << " (" << targetName << ")";
        if (agents[i].hasGuidance) {
            std::cout << " [Whisper: " << GoalToString(agents[i].suggestedGoal) << "]";
        }
        std::cout << "\n";
    }

    std::cout << "\n[WORLD CHRONICLE - RECENT EVENTS]\n";
    auto recent = GetRecentEvents(settings.defaultEventLogCount);
    if (recent.empty()) {
        std::cout << "  The world awaits its first spark...\n";
    }
    else {
        for (size_t i = 0; i < recent.size(); ++i) {
            std::cout << "  [T" << recent[i].tick << "] "
                << CategoryTag(recent[i].category)
                << recent[i].message << "\n";
        }
    }

    std::cout << "\n-----------------------------------------------------------------\n";
    std::cout << "  [Enter] +1 Tick | [F] +" << settings.fastForwardStep
        << " Ticks | [Number] +N Ticks\n";
    std::cout << "  [G] Divine Guidance | [A] Achievements | [Q] Quit to Menu\n";
    std::cout << "-----------------------------------------------------------------\n";
    std::cout << "Command: ";

}
void Sim::RenderAchievementsScreen(const Player& overseer) const {
    ClearConsole();
    std::cout << "=================================================================\n";
    std::cout << "  WORLD OVERSEER - ACHIEVEMENTS ("
        << achievement.GetUnlockedCount() << " / " << achievement.GetTotalCount() << ")\n";
    std::cout << "  Overseer Points Available: " << overseer.GetOverseerPoints() << " OP\n";
    std::cout << "=================================================================\n\n";
    const std::vector<AchievementItem>& items = achievement.GetAll();
    for (size_t i = 0; i < items.size(); ++i) {
        std::cout << (items[i].unlocked ? "[UNLOCKED] " : "[LOCKED]   ")
            << items[i].title << " (+" << items[i].rewardPoints << " OP)\n"
            << "           " << items[i].description << "\n\n";
    }
    PauseForEnter("Press Enter to return to the Observer View...");
}
std::string Sim::GoalToString(AgentGoal goal) const {
    switch (goal) {
    case AgentGoal::Forage:  return "Foraging";
    case AgentGoal::Farm:    return "Farming";
    case AgentGoal::Expand:  return "Expanding";
    case AgentGoal::Trade:   return "Trading";
    case AgentGoal::Fortify: return "Fortifying";
    case AgentGoal::Raid:    return "Raiding";
    }
    return "Idle";
}

std::string Sim::CategoryTag(EventCategory cat) const {
    switch (cat) {
    case EventCategory::Civilization: return "[CIV] ";
    case EventCategory::Conflict:     return "[WAR] ";
    case EventCategory::Nature:       return "[NAT] ";
    case EventCategory::WorldLaw:     return "[LAW] ";
    case EventCategory::Achievement:  return "[ACH] ";
    }
    return "[LOG] ";
}
void Sim::ClearConsole() const {
    std::system("cls");
}
void Sim::PauseForEnter(const std::string& prompt) const {
    std::cout << "\n" << prompt;
    std::string dummy;
    std::getline(std::cin, dummy);
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





