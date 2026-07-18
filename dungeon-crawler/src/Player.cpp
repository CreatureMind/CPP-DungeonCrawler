#include "Player.h"

int attackBonus = 5;
int expToNextLevel = 100;

void Player::move(int dx, int dy) {
    x += dx;
    y += dy;
}

void Player::takeDamage(int amount) {
    health -= amount;
    if (health < 0) health = 0;
}

void Player::addCoins(int amount) {
    coins += amount;
}

void Player::gainExp(int amount) {
    currentExp += amount;
    chechLevelUp();
}

void Player::chechLevelUp() {
    if (currentExp < expToNextLevel) return;

    if (currentExp >= expToNextLevel) {
        auto leftover = currentExp - expToNextLevel;
        level++;
        attack += attackBonus;
        currentExp = leftover;

        expToNextLevel *= 1.2f;
    }
}

void Player::collectItem(const Item& item) {
    if (item.type == ItemType::COINS) {
        coins += item.value;
    }
    else if (item.type == ItemType::POTION) {
        m_inventory.push_back(item.name);
    }
    else if (item.type == ItemType::SWORD) {
        attack += item.value;
        m_inventory.push_back(item.name);
    }
    else {
        m_inventory.push_back(item.name);
    }
}