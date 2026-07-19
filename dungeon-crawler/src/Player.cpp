#include "GameEngine.h"

int attackBonus = 5;
int expToNextLevel = 100;

void Player::move(int deltaX, int deltaY) {
    x += deltaX;
    y += deltaY;
}

void Player::takeDamage(int amount) {
    AudioBridge::trigger(GameEvent::ATTACKED);

    health -= amount;

    if (health < 0) 
        health = 0;
}

void Player::addCoins(int amount) {
    AudioBridge::trigger(GameEvent::PICKED_GOLD);
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
        
        AudioBridge::trigger(GameEvent::LEVEL_UP);

        maxHealth *= 1.1f;
        baseAttack += attackBonus;
        currentExp = leftover;

        health = maxHealth;
        expToNextLevel *= 1.2f;
    }
}

void Player::collectItem(const Item& item) {
    if (item.type == ItemType::COINS) {
        addCoins(item.value);
    }
    else if (item.type == ItemType::CHEST) {
        addCoins(item.value);
    }
    else {
        AudioBridge::trigger(GameEvent::PICKED_LOOT);

        if (item.type == ItemType::POTION) {
            potionCount++;
        }
        else if (item.type == ItemType::KEY) {
            keyCount++;
        }
        else if (item.type == ItemType::SWORD) {
            Item* newWeaponInBag = new Item(item);
            m_inventory.push_back(newWeaponInBag);

            if (m_equippedWeapon == nullptr) {
                m_equippedWeapon = newWeaponInBag;
                equippedWeaponName = newWeaponInBag->name;
                return;
            }

            if (newWeaponInBag->value > m_equippedWeapon->value) {
                m_equippedWeapon = newWeaponInBag;
                equippedWeaponName = newWeaponInBag->name;
            }
        }
    }
}

int Player::getTotalAttack() const {
    if (m_equippedWeapon != nullptr) {
        return baseAttack + m_equippedWeapon->value;
    }
    return baseAttack;
}

bool Player::useKey() {
    if (keyCount > 0) {
        keyCount--;
        return true;
    }
    return false;
}

void Player::drinkPotion() {
    if (potionCount > 0) {
        if (health >= maxHealth) return;

        potionCount--;
        health += 25;

        AudioBridge::trigger(GameEvent::DRINK_POTION);

        if (health > maxHealth) 
            health = maxHealth;
    }
}

bool Player::hasWeaponEquipped() const {
    return (m_equippedWeapon != nullptr);
}