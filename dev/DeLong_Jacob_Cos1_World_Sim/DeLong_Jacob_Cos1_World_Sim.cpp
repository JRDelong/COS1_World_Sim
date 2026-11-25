#include <iostream>
#include <string>
#include "World_Creation.h"

void ClearScreen() {
    // ANSI escape sequence: clears the screen and moves cursor to top-left (1,1)
    std::cout << "\033[2J\033[1;1H";
}

void PauseForTurn() {
    std::cout << "\nPress Enter to continue...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

int ReadInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);

        try {
            return std::stoi(input);
        }
        catch (...) {
            std::cout << "[!] Invalid input. Please enter a valid number.\n";
        }
    }
}

void HandleBiomeShop(World_Creation& world) {
    ClearScreen();
    std::cout << "--- BIOME CREATION CATALOG ---\n";
    std::cout << "1. Verdant Canopy (Cost: 500 OP)\n";
    std::cout << "   Trait: Lush Ecosystem | Bonus: +40% Pop Growth | Flaw: -20% Civ Power\n";
    std::cout << "2. Volcanic Caldera (Cost: 700 OP)\n";
    std::cout << "   Trait: Geothermal Core | Bonus: +50% Civ Power | Flaw: -40% Pop Cap & Slow Growth\n";
    std::cout << "3. Crystal Ley-Plains (Cost: 900 OP)\n";
    std::cout << "   Trait: Mana Saturated | Bonus: +35% Civ Power | Flaw: -25% Pop Cap\n";
    std::cout << "4. Sunken Archipelago (Cost: 600 OP)\n";
    std::cout << "   Trait: Vast Horizons | Bonus: +50% Pop Cap | Flaw: -15% Civ Power\n";
    std::cout << "0. Cancel\n\n";

    int pick = ReadInt("Select a biome to manifest: ");

    switch (pick) {
    case 1:
        world.CreateBiome(
            Biome("Verdant Canopy", "Lush Ecosystem", "+40% Pop Growth", "-20% Civ Power", 40, 80, 100),
            500
        );
        break;
    case 2:
        world.CreateBiome(
            Biome("Volcanic Caldera", "Geothermal Core", "+50% Civ Power", "-40% Pop Cap & Slow Growth", 10, 150, 60),
            700
        );
        break;
    case 3:
        world.CreateBiome(
            Biome("Crystal Ley-Plains", "Mana Saturated", "+35% Civ Power", "-25% Pop Cap", 25, 135, 75),
            900
        );
        break;
    case 4:
        world.CreateBiome(
            Biome("Sunken Archipelago", "Vast Horizons", "+50% Pop Cap", "-15% Civ Power", 25, 85, 150),
            600
        );
        break;
    case 0:
        std::cout << "Cancelled biome creation.\n";
        break;
    default:
        std::cout << "[!] Invalid biome choice.\n";
        break;
    }
}

void HandleRaceShop(World_Creation& world) {
    int biomeIndex = ReadInt("Enter Biome Index [0, 1, ...] to awaken sentient life in: ");

    ClearScreen();
    std::cout << "--- SENTIENT RACE CATALOG (Target Biome: [" << biomeIndex << "]) ---\n";
    std::cout << "1. Sylvan Elves (Cost: 800 OP)\n";
    std::cout << "   Trait: Nature-Bound | Preferred Biome: Verdant Canopy\n";
    std::cout << "   Bonus: +35% Civ Power | Flaw: -15% Pop Growth\n";
    std::cout << "2. Ignis-Kin (Cost: 900 OP)\n";
    std::cout << "   Trait: Forge-Born | Preferred Biome: Volcanic Caldera\n";
    std::cout << "   Bonus: +50% Era Advancement Speed | Flaw: -15% Civ Power\n";
    std::cout << "3. Aetherians (Cost: 1,100 OP)\n";
    std::cout << "   Trait: Mana-Channelers | Preferred Biome: Crystal Ley-Plains\n";
    std::cout << "   Bonus: +60% Civ Power | Flaw: -20% Pop Growth & -20% Era Speed\n";
    std::cout << "4. Tideborn Humans (Cost: 750 OP)\n";
    std::cout << "   Trait: Relentless Pioneers | Preferred Biome: Sunken Archipelago\n";
    std::cout << "   Bonus: +25% Pop Growth & +20% Era Speed | Flaw: -20% Civ Power\n";
    std::cout << "0. Cancel\n\n";

    int pick = ReadInt("Select a race to awaken: ");

    switch (pick) {
    case 1:
        world.SeedSentientRace(
            biomeIndex,
            Race("Sylvan Elves", "Nature-Bound", "+35% Civ Power", "-15% Pop Growth", "Verdant Canopy", -15, 100, 135),
            800
        );
        break;
    case 2:
        world.SeedSentientRace(
            biomeIndex,
            Race("Ignis-Kin", "Forge-Born", "+50% Era Speed", "-15% Civ Power", "Volcanic Caldera", 0, 150, 85),
            900
        );
        break;
    case 3:
        world.SeedSentientRace(
            biomeIndex,
            Race("Aetherians", "Mana-Channelers", "+60% Civ Power", "-20% Growth & -20% Era Speed", "Crystal Ley-Plains", -20, 80, 160),
            1100
        );
        break;
    case 4:
        world.SeedSentientRace(
            biomeIndex,
            Race("Tideborn Humans", "Relentless Pioneers", "+25% Growth & +20% Era Speed", "-20% Civ Power", "Sunken Archipelago", 25, 120, 80),
            750
        );
        break;
    case 0:
        std::cout << "Cancelled race awakening.\n";
        break;
    default:
        std::cout << "[!] Invalid race choice.\n";
        break;
    }
}

int main() {
    ClearScreen();
    std::string worldName;
    std::cout << "Enter a name for your newly formed world: ";
    std::getline(std::cin, worldName);

    World_Creation myWorld(worldName, 1300);

    bool running = true;
    while (running) {
        ClearScreen();
        myWorld.DisplayStatus();

        std::cout << "\n--- OVERSEER ACTIONS ---\n";
        std::cout << "1. Manifest Biome (Open Biome Catalog)\n";
        std::cout << "2. Upgrade Existing Biome\n";
        std::cout << "3. Awaken Sentient Race (Open Race Catalog)\n";
        std::cout << "4. Advance Time (1 Epoch)\n";
        std::cout << "5. Fast-Forward Time (10 Epochs)\n";
        std::cout << "6. View Overseer Achievements\n";
        std::cout << "0. Exit Simulation\n";

        int choice = ReadInt("Choose an action: ");

        switch (choice) {
        case 1:
            HandleBiomeShop(myWorld);
            PauseForTurn();
            break;
        case 2: {
            int index = ReadInt("Enter Biome Index [0, 1, ...] to upgrade: ");
            myWorld.UpgradeBiome(index);
            PauseForTurn();
            break;
        }
        case 3:
            HandleRaceShop(myWorld);
            PauseForTurn();
            break;
        case 4:
            myWorld.AdvanceTime(1);
            PauseForTurn();
            break;
        case 5:
            myWorld.AdvanceTime(10);
            PauseForTurn();
            break;
        case 6:
            ClearScreen();
            myWorld.DisplayAchievements();
            PauseForTurn();
            break;
        case 0:
            running = false;
            break;
        default:
            std::cout << "\n[!] Unknown command.\n";
            PauseForTurn();
            break;
        }
    }

    return 0;
}



