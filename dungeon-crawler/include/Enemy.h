#pragma once

struct Enemy {
    int x = 0;
    int y = 0;
    int health = 30;
    int attack = 5;
    bool isAlive = true;
    int damageTakenLastFrame = 0;
};