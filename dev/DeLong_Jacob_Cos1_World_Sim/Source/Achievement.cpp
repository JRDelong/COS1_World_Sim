#include "Achievement.h"

Achievement::Achievement() {
    RegisterDefaults();
}

void Achievement::RegisterDefaults() {
    list.push_back({ "first_settlement", "First Spark", "A sentient race settles in the world.", 10, false });
    list.push_back({ "first_expansion", "Manifest Destiny", "A civilization claims an unclaimed sector.", 15, false });
    list.push_back({ "first_trade", "Merchant Roots", "Two civilizations complete a resource trade.", 15, false });
    list.push_back({ "first_conflict", "Blood & Iron", "The first raid breaks out between factions.", 15, false });
    list.push_back({ "pop_50", "Thriving Civilization", "A single faction reaches 50 population.", 20, false });
    list.push_back({ "biome_adapted", "Hardened Survivors", "A species adapts to a non-preferred biome.", 25, false });
    list.push_back({ "multi_species", "Crowded World", "Three or more sentient factions exist at once.", 20, false });
}

std::vector<AchievementItem> Achievement::Evaluate(const WorldMilestoneStats& stats) {
    std::vector<AchievementItem> newlyUnlocked;
    AchievementItem unlockedItem;

    if (stats.activeAgents >= 1 && TryUnlock("first_settlement", unlockedItem)) {
        newlyUnlocked.push_back(unlockedItem);
    }
    if (stats.activeAgents >= 3 && TryUnlock("multi_species", unlockedItem)) {
        newlyUnlocked.push_back(unlockedItem);
    }
    if (stats.maxPopulation >= 50 && TryUnlock("pop_50", unlockedItem)) {
        newlyUnlocked.push_back(unlockedItem);
    }
    if (stats.totalExpansions >= 1 && TryUnlock("first_expansion", unlockedItem)) {
        newlyUnlocked.push_back(unlockedItem);
    }
    if (stats.totalTrades >= 1 && TryUnlock("first_trade", unlockedItem)) {
        newlyUnlocked.push_back(unlockedItem);
    }
    if (stats.totalRaids >= 1 && TryUnlock("first_conflict", unlockedItem)) {
        newlyUnlocked.push_back(unlockedItem);
    }
    if (stats.totalAdaptations >= 1 && TryUnlock("biome_adapted", unlockedItem)) {
        newlyUnlocked.push_back(unlockedItem);
    }

    return newlyUnlocked;
}

bool Achievement::TryUnlock(const std::string& achievementId, AchievementItem& outUnlocked) {
    for (size_t i = 0; i < list.size(); ++i) {
        if (list[i].id == achievementId && !list[i].unlocked) {
            list[i].unlocked = true;
            outUnlocked = list[i];
            return true;
        }
    }
    return false;
}

const std::vector<AchievementItem>& Achievement::GetAll() const {
    return list;
}

int Achievement::GetUnlockedCount() const {
    int count = 0;
    for (size_t i = 0; i < list.size(); ++i) {
        if (list[i].unlocked) {
            ++count;
        }
    }
    return count;
}

int Achievement::GetTotalCount() const {
    return static_cast<int>(list.size());
}

