#pragma once
#include "GameEvents.h"
#include <functional>

class AudioBridge {
public:
    static void trigger(GameEvent event) {
        if (s_audioCallback) {
            s_audioCallback(event);
        }
    }

    static void registerSystem(std::function<void(GameEvent)> callback) {
        s_audioCallback = callback;
    }

private:
    static inline std::function<void(GameEvent)> s_audioCallback = nullptr;
};