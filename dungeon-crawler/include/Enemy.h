#pragma once

struct Enemy {
    int x = 0;
    int y = 0;
    int health = 30;
    int attack = 5;
    bool isAlive = true;
    int damageTakenLastFrame = 0;

    char mapChar = 'M';
    std::string name = "Monster";
    int tileX = 11;
    int tileY = 0;
    int exp = 10;
};