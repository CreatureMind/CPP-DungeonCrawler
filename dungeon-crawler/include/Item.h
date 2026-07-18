#pragma once
#include <string>

enum class ItemType {
    CHEST,
    POTION,
    KEY,
    SWORD,
    COINS
};

struct Item {
    int x = 0;
    int y = 0;
    bool isAlive = true;
    ItemType type = ItemType::COINS;
    int value = 0;
    std::string name = "Loot";

    int tileX = 0;
    int tileY = 0;

    static Item createDroppedLoot(int x, int y, ItemType type);
};