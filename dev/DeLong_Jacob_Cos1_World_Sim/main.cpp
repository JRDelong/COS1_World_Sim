#include "Sim.h"
#include <chrono>
#include <conio.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace {

    std::string BiomeToString(BiomeType biome) {
        switch (biome) {
        case BiomeType::Forest:    return "Forest";
        case BiomeType::Plains:    return "Plains";
        case BiomeType::Mountains: return "Mountains";
        case BiomeType::Desert:    return "Desert";
        case BiomeType::Wetlands:  return "Wetlands";
        }
        return "Unknown";
    }

    std::string GoalToString(AgentGoal goal) {
        switch (goal) {
        case AgentGoal::Forage:  return "Foraging";
        case AgentGoal::Expand:  return "Expanding";
        case AgentGoal::Trade:   return "Trading";
        case AgentGoal::Fortify: return "Fortifying";
        case AgentGoal::Raid:    return "Raiding";
        }
        return "Idle";
    }

    std::string CategoryTag(EventCategory cat) {
        switch (cat) {
        case EventCategory::Civilization: return "[CIV] ";
        case EventCategory::Conflict:     return "[WAR] ";
        case EventCategory::Nature:       return "[NAT] ";
        case EventCategory::WorldLaw:     return "[LAW] ";
        case EventCategory::Achievement:  return "[ACH] ";
        }
        return "[LOG] ";
    }

    void ClearConsole() {
        std::cout << "\033[2J\033[1;1H";
    }

    void SeedInitialWorld(Sim& sim) {
        sim.AddSector("Emerald Reach", BiomeType::Forest, 80, 40);
        sim.AddSector("Sunlit Steppe", BiomeType::Plains, 70, 35);
        sim.AddSector("Ironcrag Peaks", BiomeType::Mountains, 25, 90);
        sim.AddSector("Ashen Dunes", BiomeType::Desert, 15, 50);

        sim.AddAgent("Sylvan Kin", "Elves", BiomeType::Forest, 0);
        sim.AddAgent("Stoneborn Clan", "Dwarves", BiomeType::Mountains, 2);
    }

    void RenderAchievementsScreen(const Sim& sim) {
        ClearConsole();
        const Achievement& ach = sim.GetAchievement();

        std::cout << "=================================================================\n";
        std::cout << "  WORLD KEEPER - ACHIEVEMENTS ("
            << ach.GetUnlockedCount() << " / " << ach.GetTotalCount() << ")\n";
        std::cout << "  Overseer Points Available: " << sim.GetOverseerPoints() << " OP\n";
        std::cout << "=================================================================\n\n";

        const std::vector<AchievementItem>& items = ach.GetAll();
        for (size_t i = 0; i < items.size(); ++i) {
            std::cout << (items[i].unlocked ? "[UNLOCKED] " : "[LOCKED]   ")
                << items[i].title << " (+" << items[i].rewardPoints << " OP)\n"
                << "           " << items[i].description << "\n\n";
        }

        std::cout << "Press any key to return to the Observer View...";
        _getch();
    }

    void RenderObserverHUD(const Sim& sim, int simSpeed) {
        ClearConsole();

        std::string speedLabel = "PAUSED";
        if (simSpeed == 1) {
            speedLabel = "1x (Normal)";
        }
        else if (simSpeed == 5) {
            speedLabel = "5x (Fast)";
        }

        const Achievement& ach = sim.GetAchievement();

        std::cout << "=================================================================\n";
        std::cout << "  WORLD KEEPER - OBSERVER CONSOLE\n";
        std::cout << "  Tick: " << sim.GetCurrentTick()
            << "  |  Speed: " << speedLabel
            << "  |  Overseer Points: " << sim.GetOverseerPoints() << " OP"
            << "  |  Achievements: " << ach.GetUnlockedCount() << "/" << ach.GetTotalCount() << "\n";
        std::cout << "=================================================================\n";

        const std::vector<WorldModifier>& mods = sim.GetActiveModifiers();
        std::cout << "\n[ACTIVE WORLD LAWS & PHENOMENA]\n";
        if (mods.empty()) {
            std::cout << "  None enacted yet.\n";
        }
        else {
            for (size_t i = 0; i < mods.size(); ++i) {
                std::cout << "  - " << mods[i].name;
                if (!mods[i].isPermanent) {
                    std::cout << " (" << mods[i].remainingTicks << " ticks left)";
                }
                else {
                    std::cout << " (Permanent Law)";
                }
                std::cout << "\n";
            }
        }

        const std::vector<Sector>& sectors = sim.GetSectors();
        const std::vector<Agent>& agents = sim.GetAgents();

        std::cout << "\n[WORLD SECTORS]\n";
        for (size_t i = 0; i < sectors.size(); ++i) {
            std::string ownerName = "Unclaimed";
            if (sectors[i].ownerAgentId >= 0 && sectors[i].ownerAgentId < static_cast<int>(agents.size())) {
                ownerName = agents[sectors[i].ownerAgentId].name;
            }
            std::cout << "  #" << sectors[i].id << " " << sectors[i].name
                << " (" << BiomeToString(sectors[i].biome) << ")"
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
            std::cout << "  * " << agents[i].name << " (" << agents[i].species << ")"
                << " | Pop: " << agents[i].population
                << " | Food: " << agents[i].food
                << " | Mat: " << agents[i].materials
                << " | Aggr: " << agents[i].aggression
                << " | Goal: " << GoalToString(agents[i].currentGoal) << "\n";
        }

        std::cout << "\n[WORLD CHRONICLE - RECENT EVENTS]\n";
        std::vector<SimEvent> recent = sim.GetRecentEvents(10);
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
        std::cout << "  Controls: [P] Pause  [1] 1x Speed  [2] 5x Speed  [A] Achievements  [Q] Quit\n";
        std::cout << "-----------------------------------------------------------------\n";
    }

    void RunObserverSession(Sim& sim) {
        int simSpeed = 1;
        bool running = true;
        bool needsRedraw = true;

        auto lastTickTime = std::chrono::steady_clock::now();

        while (running) {
            if (_kbhit()) {
                int ch = _getch();
                if (ch == 'p' || ch == 'P') {
                    simSpeed = 0;
                    needsRedraw = true;
                }
                else if (ch == '1') {
                    simSpeed = 1;
                    needsRedraw = true;
                }
                else if (ch == '2') {
                    simSpeed = 5;
                    needsRedraw = true;
                }
                else if (ch == 'a' || ch == 'A') {
                    RenderAchievementsScreen(sim);
                    lastTickTime = std::chrono::steady_clock::now();
                    needsRedraw = true;
                }
                else if (ch == 'q' || ch == 'Q') {
                    running = false;
                }
            }

            if (simSpeed > 0) {
                auto now = std::chrono::steady_clock::now();
                auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTickTime).count();

                int intervalMs = (simSpeed == 5) ? 250 : 1000;
                if (elapsedMs >= intervalMs) {
                    sim.StepSingleTick();
                    lastTickTime = now;
                    needsRedraw = true;
                }
            }

            if (needsRedraw) {
                RenderObserverHUD(sim, simSpeed);
                needsRedraw = false;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }

} 

int main() {
    bool appOpen = true;

    while (appOpen) {
        ClearConsole();
        std::cout << "=========================================\n";
        std::cout << "              WORLD KEEPER               \n";
        std::cout << "=========================================\n";
        std::cout << "  1. Start New World Observation\n";
        std::cout << "  2. Exit\n";
        std::cout << "-----------------------------------------\n";
        std::cout << "Select an option: ";

        int choice = 0;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (choice == 1) {
            Sim sim;
            SeedInitialWorld(sim);
            RunObserverSession(sim);
        }
        else if (choice == 2) {
            appOpen = false;
        }
    }

    return 0;
}
