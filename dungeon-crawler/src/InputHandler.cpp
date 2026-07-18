#include "InputHandler.h"
#include "raylib.h"

Action pollAction(const Game& game) {
    if (WindowShouldClose()) return Action::QUIT;

    int key = GetKeyPressed();
    while (key != 0) {
        //TraceLog(LOG_INFO, "Key detected by Raylib: %d", key);

        Action a = game.mapKeyToAction(key);
        if (a != Action::NONE) return a;
        key = GetKeyPressed();
    }
    return Action::NONE;
}
