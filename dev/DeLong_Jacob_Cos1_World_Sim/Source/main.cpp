#include "MainMenu.h"
#include "Player.h"
#include "Sim.h"
#include "World_Creation.h"

int main() {
    GameSettings settings;
    MainMenu menu;

    while (true) {
        MenuAction action = menu.Show(settings);

        if (action == MenuAction::StartNewWorld) {
            Player overseer(1000);
            Sim sim;
            World_Creation worldCreator;

            if (worldCreator.RunGenesis(sim, overseer)) {
                sim.RunObserverSession(overseer, settings);
            }
        }
        else if (action == MenuAction::ExitGame) {
            break;
        }
    }

    return 0;
}








