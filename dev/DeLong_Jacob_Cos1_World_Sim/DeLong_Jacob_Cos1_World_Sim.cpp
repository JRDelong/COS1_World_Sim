#include <iostream>
#include <string>
#include "World_Creation.h"


int main()
{
    std::string worldName;
    std::cout << "Enter a name for your newly formed world: ";
    std::getline(std::cin, worldName);

    World_Creation myWorld(worldName, 1000);

    bool running = true;
    while (running) {
        myWorld.DisplayStatus();
        std::cout << "\n--- Overseer Actions ---\n";
        std::cout << "1. Manifest Biome (Cost: 500 Energy)\n";
        std::cout << "2. Upgrade Existing Biome\n";
        std::cout << "3. Advance Time (1 Epoch)\n";
        std::cout << "4. Fast-Forward time (10 Epochs)\n";
        std::cout << "0. Exit Simulation\n";
        std::cout << "Choose an action: ";

        int choice;
        switch (choice)
        {
        case 1:
        {
            std::cin.ignore(10000, '\n');
            std::string biomeName;
            std::cout << "Enter Biome Name (e.g. Forest, Ashlands, Valley): ";
            myWorld.CreateBiome(biomeName, 25, 500);
            break;
        }
        case 2:
        {
            int index;
            std::cout << "Enter Biome Index [0, 1, ...] to upgrade: ";
            std::cin >> index;
            myWorld.UpgradeBiome(index);
            break;
        }
        case 3:
            myWorld.AdvanceTime(1);
            break;
        case 4:
            myWorld.AdvanceTime(1);
            break;
        case 0:
            running = false;
            break;
        default:
            std::cout << "\n[!] Unknown Command.\n";
            break;
        }
    }
    return 0;
}