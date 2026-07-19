#pragma once
#include "GameEvents.h"
#include "raylib.h"
#include <vector>

class AudioSystem {
public:
    AudioSystem();
    ~AudioSystem();

    void processEvent(GameEvent event);
    void updateMusicStream();

private:
    Music m_bgMusic;

    Sound m_sndSword;
    Sound m_sndCoin;
    Sound m_sndPickup;
    Sound m_sndPotion;
    Sound m_sndDoor;
    Sound m_sndLevelUp;

    std::vector<Sound> m_footsteps;
    std::vector<Sound> m_hits;

    void playRandomFootstep();
    void playRandomHit();
};
