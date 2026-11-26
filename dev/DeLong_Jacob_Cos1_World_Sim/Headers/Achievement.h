#pragma once
#include <string>
#include <vector>

struct AchievementItem {
    std::string id;
    std::string title;
    std::string description;
    int rewardPoints = 0;
    bool unlocked = false;
};

struct WorldMilestoneStats {
    int activeAgents = 0;
    int maxPopulation = 0;
    int totalSectors = 0;
    int totalExpansions = 0;
    int totalTrades = 0;
    int totalRaids = 0;
    int totalAdaptations = 0;
};

class Achievement {
public:
    Achievement();

    std::vector<AchievementItem> Evaluate(const WorldMilestoneStats& stats);

    bool TryUnlock(const std::string& achievementId, AchievementItem& outUnlocked);

    const std::vector<AchievementItem>& GetAll() const;
    int GetUnlockedCount() const;
    int GetTotalCount() const;

private:
    std::vector<AchievementItem> list;
    void RegisterDefaults();
};


