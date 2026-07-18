#pragma once
#include <vector>
#include <string>
#include "Item.h"

struct Player {
    int x = 0;
    int y = 0;
    int health = 100;
    int attack = 15;
    int coins = 0;
    int level = 1;
    int currentExp = 0;

    std::vector<std::string> m_inventory;

    void move(int deltaX, int deltaY);
    void takeDamage(int amount);
    void heal(int amount);
    void addCoins(int amount);
    void gainExp(int amount);
    void chechLevelUp();
    void collectItem(const Item& item);
};