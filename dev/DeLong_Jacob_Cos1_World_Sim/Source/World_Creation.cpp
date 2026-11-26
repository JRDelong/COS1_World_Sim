#include "World_Creation.h"
#include <cstdlib>
#include <iostream>
#include <vector>

bool World_Creation::RunGenesis(Sim& sim, Player& overseer) {
    if (!RunBiomeStep(sim, overseer)) {
        return false;
    }
    return RunRaceStep(sim, overseer);
}

bool World_Creation::RunBiomeStep(Sim& sim, Player& overseer) {
    std::vector<BiomePreset> biomeCatalog = Biome::GetStarterCatalog();
    std::string statusMsg;

    while (true) {
        ClearConsole();
        std::cout << "=================================================================\n";
        std::cout << "  WORLD GENESIS - STEP 1 OF 2: MANIFEST A BIOME\n";
        std::cout << "  Overseer Points: " << overseer.GetOverseerPoints() << " OP"
            << "  |  Biomes Created: " << sim.GetSectors().size() << "\n";
        std::cout << "=================================================================\n\n";
        std::cout << "  The world is currently an empty void. Manifest at least one\n";
        std::cout << "  biome before awakening sentient life.\n\n";

        for (size_t i = 0; i < biomeCatalog.size(); ++i) {
            std::cout << "  [" << (i + 1) << "] " << biomeCatalog[i].name
                << " (" << Biome::ToString(biomeCatalog[i].type) << ") - "
                << biomeCatalog[i].costOP << " OP\n"
                << "      " << biomeCatalog[i].summary << "\n\n";
        }

        if (sim.HasAnySector()) {
            std::cout << "  [C] Continue to Step 2 (Awaken Sentient Race)\n";
        }
        std::cout << "  [Q] Cancel & Return to Main Menu\n";
        std::cout << "-----------------------------------------------------------------\n";

        if (!statusMsg.empty()) {
            std::cout << ">> " << statusMsg << "\n\n";
        }

        std::cout << "Select an option: ";
        std::string input;
        if (!std::getline(std::cin, input)) {
            return false;
        }

        if (input == "q" || input == "Q") {
            return false;
        }
        if ((input == "c" || input == "C") && sim.HasAnySector()) {
            return true;
        }

        char* endPtr = nullptr;
        long choice = std::strtol(input.c_str(), &endPtr, 10);
        if (endPtr != input.c_str() && *endPtr == '\0' && choice >= 1 && choice <= static_cast<long>(biomeCatalog.size())) {
            const BiomePreset& selected = biomeCatalog[choice - 1];
            if (!overseer.SpendPoints(selected.costOP)) {
                statusMsg = "Insufficient Overseer Points! Need " + std::to_string(selected.costOP) + " OP.";
            }
            else {
                std::string uniqueName = selected.name + " #" + std::to_string(sim.GetSectors().size() + 1);
                sim.AddSector(uniqueName, selected.type, selected.initialFood, selected.initialMaterials);
                statusMsg = "Manifested " + uniqueName + "! Buy another biome or press [C] to continue.";
            }
        }
        else {
            statusMsg = sim.HasAnySector()
                ? "Enter a biome number, [C] to continue, or [Q] to cancel."
                : "You must buy at least one biome first!";
        }
    }
}

bool World_Creation::RunRaceStep(Sim& sim, Player& overseer) {
    std::vector<RacePreset> raceCatalog = Race::GetStarterCatalog();
    std::string statusMsg;

    while (true) {
        ClearConsole();
        const std::vector<Sector>& sectors = sim.GetSectors();

        int unclaimedCount = 0;
        for (size_t i = 0; i < sectors.size(); ++i) {
            if (sectors[i].ownerAgentId == -1) {
                ++unclaimedCount;
            }
        }

        std::cout << "=================================================================\n";
        std::cout << "  WORLD GENESIS - STEP 2 OF 2: AWAKEN A SENTIENT RACE\n";
        std::cout << "  Overseer Points: " << overseer.GetOverseerPoints() << " OP"
            << "  |  Races Awakened: " << sim.GetAgents().size()
            << "  |  Open Sectors: " << unclaimedCount << "\n";
        std::cout << "=================================================================\n\n";
        std::cout << "  Awaken at least one sentient race in an unclaimed biome sector\n";
        std::cout << "  to begin watching the world evolve.\n\n";

        for (size_t i = 0; i < raceCatalog.size(); ++i) {
            std::cout << "  [" << (i + 1) << "] " << raceCatalog[i].factionName
                << " (" << raceCatalog[i].speciesName << " | Prefers: "
                << Biome::ToString(raceCatalog[i].preferredBiome) << ") - "
                << raceCatalog[i].costOP << " OP\n"
                << "      " << raceCatalog[i].summary << "\n\n";
        }

        if (sim.HasAnyAgent()) {
            std::cout << "  [S] Start World Observation\n";
        }
        std::cout << "  [Q] Cancel & Return to Main Menu\n";
        std::cout << "-----------------------------------------------------------------\n";

        if (!statusMsg.empty()) {
            std::cout << ">> " << statusMsg << "\n\n";
        }

        std::cout << "Select an option: ";
        std::string input;
        if (!std::getline(std::cin, input)) {
            return false;
        }

        if (input == "q" || input == "Q") {
            return false;
        }
        if ((input == "s" || input == "S") && sim.HasAnyAgent()) {
            return true;
        }

        char* endPtr = nullptr;
        long choice = std::strtol(input.c_str(), &endPtr, 10);
        if (endPtr != input.c_str() && *endPtr == '\0' && choice >= 1 && choice <= static_cast<long>(raceCatalog.size())) {
            if (unclaimedCount == 0) {
                statusMsg = "No unclaimed biomes left! Press [S] to begin observation.";
                continue;
            }

            const RacePreset& selected = raceCatalog[choice - 1];
            if (overseer.GetOverseerPoints() < selected.costOP) {
                statusMsg = "Insufficient Overseer Points! Need " + std::to_string(selected.costOP) + " OP.";
                continue;
            }

            int chosenSectorId = -1;
            for (size_t i = 0; i < sectors.size(); ++i) {
                if (sectors[i].ownerAgentId == -1) {
                    if (chosenSectorId == -1) {
                        chosenSectorId = sectors[i].id;
                    }
                    if (sectors[i].biome == selected.preferredBiome) {
                        chosenSectorId = sectors[i].id;
                        break;
                    }
                }
            }

            if (chosenSectorId != -1 && overseer.SpendPoints(selected.costOP)) {
                sim.AddAgent(selected.factionName, selected.speciesName, selected.preferredBiome, chosenSectorId);
                statusMsg = "Awakened " + selected.factionName + " in " + sectors[chosenSectorId].name + "! Press [S] to begin or add another.";
            }
        }
        else {
            statusMsg = sim.HasAnyAgent()
                ? "Enter a race number, [S] to start observation, or [Q] to cancel."
                : "You must awaken at least one race before starting!";
        }
    }
}

void World_Creation::ClearConsole() const {
    std::system("cls");
}




