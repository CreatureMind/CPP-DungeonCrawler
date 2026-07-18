#pragma once
#include <string>
#include "Item.h"

struct Enemy {
    int x = 0;
    int y = 0;
    int health = 30;
    int attack = 5;
    int expReward = 10;
    bool isAlive = true;
    int damageTakenLastFrame = 0;
    std::string lootDropType = "coins";

    char mapChar = 'M';
    std::string name = "Monster";
    int tileX = 11;
    int tileY = 0;

    void takeDamage(int amount);
    ItemType getLootTypeEnum() const;
};