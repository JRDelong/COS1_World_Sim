#pragma once

#include "Biome.h"
#include "Player.h"
#include "Race.h"
#include "Sim.h"
#include <string>

class World_Creation {
public:
    World_Creation() = default;

    // Runs Step 1 (Buy Biome) -> Step 2 (Buy Race). Returns true when ready to observe.
    bool RunGenesis(Sim& sim, Player& overseer);

private:
    bool RunBiomeStep(Sim& sim, Player& overseer);
    bool RunRaceStep(Sim& sim, Player& overseer);
    void ClearConsole() const;
};





