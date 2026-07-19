#pragma once
#include <vector>
#include <string>
#include "Item.h"

struct Player {
    int x = 0;
    int y = 0;
    int health = 100;
    int maxHealth = 100;
    int baseAttack = 15;
    int coins = 0;
    int level = 1;
    int currentExp = 0;

    // inventory
    int keyCount = 0;
    int potionCount = 0;
    std::vector<Item*> m_inventory;
    Item* m_equippedWeapon = nullptr;
    std::string equippedWeaponName = "Fists";

    ~Player() {
        for (Item* item : m_inventory) {
            delete item;
        }
    }

    void move(int deltaX, int deltaY);
    void takeDamage(int amount);
    void heal(int amount);
    void addCoins(int amount);
    void gainExp(int amount);
    void chechLevelUp();
    void collectItem(const Item& item);
    bool useKey();
    void drinkPotion();
    int getTotalAttack() const;
    bool hasWeaponEquipped() const;
};