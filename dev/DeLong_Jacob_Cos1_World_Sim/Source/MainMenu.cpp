#include "MainMenu.h"
#include <cstdlib>
#include <iostream>

MainMenu::MainMenu()
    : gameTitle("WORLD OVERSEER"),
    subtitle("Indirect Simulation & Chronicle"),
    boxWidth(47) {
}

MenuAction MainMenu::Show(GameSettings& settings) {
    while (true) {
        ClearConsole();
        PrintBanner(gameTitle);

        std::vector<std::string> options;
        options.push_back("Singleplayer");
        options.push_back("Multiplayer");
        options.push_back("Options");
        options.push_back("Exit");

        PrintMenuList(options);
        std::string choice = PromptChoice();

        if (choice == "1") {
            if (ShowSingleplayerMenu()) {
                return MenuAction::StartNewWorld;
            }
        }
        else if (choice == "2") {
            ShowMultiplayerMenu();
        }
        else if (choice == "3") {
            ShowOptionsMenu(settings);
        }
        else if (choice == "4" || choice == "q" || choice == "Q") {
            return MenuAction::ExitGame;
        }
    }
}

bool MainMenu::ShowSingleplayerMenu() {
    while (true) {
        ClearConsole();
        PrintBanner("SINGLEPLAYER");

        std::vector<std::string> options;
        options.push_back("Start New World Observation");
        options.push_back("Load Saved World (Coming Soon)");
        options.push_back("Back to Main Menu");

        PrintMenuList(options);
        std::string choice = PromptChoice();

        if (choice == "1") {
            return true;
        }
        else if (choice == "2") {
            std::vector<std::string> notes;
            notes.push_back("[Placeholder] Save/Load profile system");
            notes.push_back("will be connected here.");
            ShowPlaceholderScreen("LOAD SAVED WORLD", notes);
        }
        else if (choice == "3" || choice == "q" || choice == "Q") {
            return false;
        }
    }
}

void MainMenu::ShowMultiplayerMenu() {
    while (true) {
        ClearConsole();
        PrintBanner("MULTIPLAYER");

        std::vector<std::string> options;
        options.push_back("Host Shared Pantheon World (Coming Soon)");
        options.push_back("Join Shared Pantheon World (Coming Soon)");
        options.push_back("Local Hotseat Overseers (Coming Soon)");
        options.push_back("Back to Main Menu");

        PrintMenuList(options);
        std::string choice = PromptChoice();

        if (choice == "1" || choice == "2" || choice == "3") {
            std::vector<std::string> notes;
            notes.push_back("[Placeholder] Multi-Overseer world sync");
            notes.push_back("and competing World Laws coming soon.");
            ShowPlaceholderScreen("MULTIPLAYER MODE", notes);
        }
        else if (choice == "4" || choice == "q" || choice == "Q") {
            return;
        }
    }
}

void MainMenu::ShowOptionsMenu(GameSettings& settings) {
    while (true) {
        ClearConsole();
        PrintBanner("OPTIONS");

        std::vector<std::string> options;
        options.push_back("Chronicle Log Lines Shown : " + std::to_string(settings.defaultEventLogCount));
        options.push_back("Fast-Forward [F] Step     : " + std::to_string(settings.fastForwardStep) + " ticks");
        options.push_back("Reset Options to Default");
        options.push_back("Back to Main Menu");

        PrintMenuList(options);
        std::string choice = PromptChoice();

        if (choice == "1") {
            if (settings.defaultEventLogCount == 10) {
                settings.defaultEventLogCount = 15;
            }
            else if (settings.defaultEventLogCount == 15) {
                settings.defaultEventLogCount = 20;
            }
            else {
                settings.defaultEventLogCount = 10;
            }
        }
        else if (choice == "2") {
            if (settings.fastForwardStep == 10) {
                settings.fastForwardStep = 25;
            }
            else if (settings.fastForwardStep == 25) {
                settings.fastForwardStep = 50;
            }
            else {
                settings.fastForwardStep = 10;
            }
        }
        else if (choice == "3") {
            settings = GameSettings();
        }
        else if (choice == "4" || choice == "q" || choice == "Q") {
            return;
        }
    }
}

void MainMenu::ShowPlaceholderScreen(const std::string& header, const std::vector<std::string>& lines) const {
    ClearConsole();
    PrintBanner(header);

    for (size_t i = 0; i < lines.size(); ++i) {
        std::cout << "  " << lines[i] << "\n";
    }

    PrintDivider('-');
    PauseForEnter();
}

void MainMenu::ClearConsole() const {
    std::system("cls");
}

void MainMenu::PrintBanner(const std::string& headerText) const {
    PrintDivider('=');

    int padding = (boxWidth - static_cast<int>(headerText.size())) / 2;
    if (padding < 0) {
        padding = 0;
    }
    std::cout << std::string(padding, ' ') << headerText << "\n";

    if (headerText == gameTitle && !subtitle.empty()) {
        int subPad = (boxWidth - static_cast<int>(subtitle.size())) / 2;
        if (subPad < 0) {
            subPad = 0;
        }
        std::cout << std::string(subPad, ' ') << subtitle << "\n";
    }

    PrintDivider('=');
}

void MainMenu::PrintMenuList(const std::vector<std::string>& options) const {
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << "  [" << (i + 1) << "] " << options[i] << "\n";
    }
    PrintDivider('-');
}

void MainMenu::PrintDivider(char fillChar) const {
    std::cout << std::string(boxWidth, fillChar) << "\n";
}

std::string MainMenu::PromptChoice(const std::string& promptText) const {
    std::cout << promptText;
    std::string input;
    if (!std::getline(std::cin, input)) {
        return "q";
    }
    return input;
}

void MainMenu::PauseForEnter(const std::string& promptText) const {
    std::cout << "\n" << promptText;
    std::string dummy;
    std::getline(std::cin, dummy);
}
