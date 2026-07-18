#include "GameEngine.h"

void resolveAttack(Player& player, Enemy& enemy) {
    if (!enemy.isAlive) return;

    // Apply player damage to the enemy
    enemy.health -= player.attack;
    enemy.damageTakenLastFrame = player.attack;

    if (enemy.health <= 0) {
        enemy.isAlive = false;
    }
    else {
        // Counter-attack: Enemy attacks player back
        player.health -= enemy.attack;
    }
}