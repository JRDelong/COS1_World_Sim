#pragma once

class Player {
public:
    explicit Player(int startingOP = 1000) : overseerPoints(startingOP) {}

    int GetOverseerPoints() const { return overseerPoints; }
    void EarnPoints(int amount) { if (amount > 0) overseerPoints += amount; }
    bool SpendPoints(int amount) {
        if (amount < 0 || overseerPoints < amount) return false;
        overseerPoints -= amount;
        return true;
    }

private:
    int overseerPoints = 1000;
};


