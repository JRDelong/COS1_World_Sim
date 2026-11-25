#include <iostream>
#include <string>
#include "World_Creation.h"

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
    std::cout << "\n--- BIOME CREATION CATALOG ---\n";
    std::cout << "1. Verdant Canopy (Cost: 500 Energy)\n";
    std::cout << "   Trait: Lush Ecosystem | Bonus: +40% Pop Growth | Flaw: -20% Energy Yield\n";
    std::cout << "2. Volcanic Caldera (Cost: 700 Energy)\n";
    std::cout << "   Trait: Geothermal Core | Bonus: +50% Energy Yield | Flaw: -40% Pop Cap & Slow Growth\n";
    std::cout << "3. Crystal Ley-Plains (Cost: 900 Energy)\n";
    std::cout << "   Trait: Mana Saturated | Bonus: +35% Energy Yield | Flaw: -25% Pop Cap\n";
    std::cout << "4. Sunken Archipelago (Cost: 600 Energy)\n";
    std::cout << "   Trait: Vast Horizons | Bonus: +50% Pop Cap | Flaw: -15% Energy Yield\n";
    std::cout << "0. Cancel\n";

    int pick = ReadInt("Select a biome to manifest: ");

    switch (pick) {
    case 1:
        world.CreateBiome(
            Biome("Verdant Canopy", "Lush Ecosystem", "+40% Pop Growth", "-20% Energy Yield", 40, 80, 100),
            500
        );
        break;
    case 2:
        world.CreateBiome(
            Biome("Volcanic Caldera", "Geothermal Core", "+50% Energy Yield", "-40% Pop Cap & Slow Growth", 10, 150, 60),
            700
        );
        break;
    case 3:
        world.CreateBiome(
            Biome("Crystal Ley-Plains", "Mana Saturated", "+35% Energy Yield", "-25% Pop Cap", 25, 135, 75),
            900
        );
        break;
    case 4:
        world.CreateBiome(
            Biome("Sunken Archipelago", "Vast Horizons", "+50% Pop Cap", "-15% Energy Yield", 25, 85, 150),
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

int main() {
    std::string worldName;
    std::cout << "Enter a name for your newly formed world: ";
    std::getline(std::cin, worldName);

    World_Creation myWorld(worldName, 1500);

    bool running = true;
    while (running) {
        myWorld.DisplayStatus();
        std::cout << "\n--- KEEPER ACTIONS ---\n";
        std::cout << "1. Manifest Biome (Open Biome Catalog)\n";
        std::cout << "2. Upgrade Existing Biome\n";
        std::cout << "3. Awaken Sentient Race (Cost: 800 Energy)\n";
        std::cout << "4. Advance Time (1 Epoch)\n";
        std::cout << "5. Fast-Forward Time (10 Epochs)\n";
        std::cout << "0. Exit Simulation\n";

        int choice = ReadInt("Choose an action: ");

        switch (choice) {
        case 1:
            HandleBiomeShop(myWorld);
            break;
        case 2: {
            int index = ReadInt("Enter Biome Index [0, 1, ...] to upgrade: ");
            myWorld.UpgradeBiome(index);
            break;
        }
        case 3: {
            int index = ReadInt("Enter Biome Index to seed sentient life: ");
            std::string raceName;
            std::cout << "Enter Race Name (e.g., Elves, Dwarves, Humans, Draconians): ";
            std::getline(std::cin, raceName);

            std::cout << "Select Species Trait:\n";
            std::cout << "  1. Arcane-Born (+50% World Energy Yield)\n";
            std::cout << "  2. Industrious (+50% Faster Era Advancement)\n";
            std::cout << "  3. Prolific (+20% Faster Population Growth)\n";
            int traitChoice = ReadInt("Choose trait (1-3): ");

            std::string trait = "Adaptable";
            if (traitChoice == 1) trait = "Arcane-Born";
            else if (traitChoice == 2) trait = "Industrious";
            else if (traitChoice == 3) trait = "Prolific";

            myWorld.SeedSentientRace(index, raceName, trait, 800);
            break;
        }
        case 4:
            myWorld.AdvanceTime(1);
            break;
        case 5:
            myWorld.AdvanceTime(10);
            break;
        case 0:
            running = false;
            break;
        default:
            std::cout << "\n[!] Unknown command.\n";
            break;
        }
    }

    return 0;
}

