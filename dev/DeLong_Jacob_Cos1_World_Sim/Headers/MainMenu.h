#pragma once
#include <string>
#include <vector>

struct GameSettings {
    int defaultEventLogCount = 10;
    int fastForwardStep = 10;
};

enum class MenuAction {
    StartNewWorld,
    ExitGame
};

class MainMenu {
public:
    MainMenu();

    MenuAction Show(GameSettings& settings);

private:
    std::string gameTitle;
    std::string subtitle;
    int boxWidth = 45;

    bool ShowSingleplayerMenu();
    void ShowMultiplayerMenu();
    void ShowOptionsMenu(GameSettings& settings);
    void ShowPlaceholderScreen(const std::string& header, const std::vector<std::string>& lines) const;

    void ClearConsole() const;
    void PrintBanner(const std::string& headerText) const;
    void PrintMenuList(const std::vector<std::string>& options) const;
    void PrintDivider(char fillChar) const;
    std::string PromptChoice(const std::string& promptText = "Select an option: ") const;
    void PauseForEnter(const std::string& promptText = "Press Enter to return...") const;
};


