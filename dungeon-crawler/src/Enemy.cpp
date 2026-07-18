#include "Enemy.h"
#include <cstdlib>

void Enemy::takeDamage(int amount) {
    if (!isAlive) return;
    health -= amount;
    damageTakenLastFrame = amount;
    if (health <= 0) {
        isAlive = false;
    }
}

ItemType Enemy::getLootTypeEnum() const {
    if (lootDropType == "potion") return ItemType::POTION;
    if (lootDropType == "key")    return ItemType::KEY;
    if (lootDropType == "sword")  return ItemType::SWORD;
    if (lootDropType == "coins")  return ItemType::COINS;
    if (lootDropType == "chest")  return ItemType::CHEST;

    int roll = std::rand() % 100;

    if (roll < 60) {
        return ItemType::COINS;  // 0 to 59
    }
    else if (roll < 90) {
        return ItemType::POTION; // 60 to 89
    }
    else {
        return ItemType::SWORD;  // 90 to 99
    }
}