#include "Item.h"
#include <raylib.h>

Item Item::createDroppedLoot(int x, int y, ItemType type) {
    Item item;
    item.x = x;
    item.y = y;
    item.isAlive = true;
    item.type = type;

    // Assign names and sheet coordinates based on what dropped
    switch (type) {
    case ItemType::POTION:
        item.name = "Health Potion";
        item.value = 25; // Heals for 25 HP
        item.tileX = 7;
        item.tileY = 8;
        break;
    case ItemType::KEY:
        item.name = "Dungeon Key";
        item.tileX = 10;
        item.tileY = 5;
        break;
    case ItemType::SWORD:
        item.name = "Steel Sword";
        item.value = 5;  // Adds +5 to attack power
        item.tileX = 6;
        item.tileY = 4;
        break;
    case ItemType::COINS:
        item.name = "Gold Coins";
        item.value = GetRandomValue(1, 10);
        item.tileX = 8;
        item.tileY = 5;
        break;
    case ItemType::CHEST:
        item.name = "Treasure Chest";
        item.value = 100;
        item.tileX = 9;
        item.tileY = 3;
        break;
    }
    return item;
}