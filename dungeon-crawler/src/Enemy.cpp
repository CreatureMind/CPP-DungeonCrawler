#include "GameEngine.h"
#include <cstdlib>

void Enemy::takeDamage(int amount) {
    if (!isAlive) return;

    health -= amount;
    damageTakenLastFrame = amount;

    if (health <= 0) {
        isAlive = false;
        AudioBridge::trigger(GameEvent::ATTACKED);
    }
    else {
        AudioBridge::trigger(GameEvent::ATTACKED);
    }
}

void Enemy::updateAI(int playerX, int playerY, int& outTargetX, int& outTargetY) {
    outTargetX = x;
    outTargetY = y;

    if (!isAlive) return;

    Vector2 enemyPos = { (float)x, (float)y };
    Vector2 playerPos = { (float)playerX, (float)playerY };

    float distance = Vector2Distance(enemyPos, playerPos);

    if (distance > 2.5f) {
        return;
    }

    int diffX = playerX - x;
    int diffY = playerY - y;

    if (std::abs(diffX) >= std::abs(diffY)) {
        outTargetX += (diffX > 0) ? 1 : -1;
    }
    else {
        outTargetY += (diffY > 0) ? 1 : -1;
    }
}

ItemType Enemy::getLootTypeEnum() const {
    if (lootDropType == "potion") return ItemType::POTION;
    if (lootDropType == "key") return ItemType::KEY;
    if (lootDropType == "sword") return ItemType::SWORD;
    if (lootDropType == "coins") return ItemType::COINS;
    if (lootDropType == "chest") return ItemType::CHEST;

    int roll = std::rand() % 100;

    if (roll < 60) 
        return ItemType::COINS;  // 0 to 59
    else if (roll < 90) 
        return ItemType::POTION; // 60 to 
    else 
        return ItemType::SWORD;  // 90 to 
}