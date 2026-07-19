#include "GameEngine.h"
#include <cstdlib>
#include <fstream>

AudioSystem::AudioSystem() {
    InitAudioDevice();

    auto loadChecked = [&](const char* path) {
        std::ifstream chk(path);
        if (!chk.good()) {
            TraceLog(LOG_WARNING, "Audio file not found: %s", path);
        }
        return LoadSound(path);
    };

    m_sndSword = LoadSound("assets/audio/sounds/player/sword-attack.wav");
    m_sndCoin = loadChecked("assets/audio/sounds/player/coin.wav");
    m_sndPickup = loadChecked("assets/audio/sounds/player/pickup.wav");
    m_sndPotion = loadChecked("assets/audio/sounds/player/potion.wav");
    m_sndDoor = loadChecked("assets/audio/sounds/player/door-open.wav");
    m_sndLevelUp = loadChecked("assets/audio/sounds/player/level-up.wav");

    m_footsteps.push_back(loadChecked("assets/audio/sounds/player/footstep-1.wav"));
    m_footsteps.push_back(loadChecked("assets/audio/sounds/player/footstep-2.wav"));
    m_footsteps.push_back(loadChecked("assets/audio/sounds/player/footstep-3.wav"));

    m_hits.push_back(loadChecked("assets/audio/sounds/player/hit-1.wav"));
    m_hits.push_back(loadChecked("assets/audio/sounds/player/hit-2.wav"));
    m_hits.push_back(loadChecked("assets/audio/sounds/player/hit-3.wav"));

    m_bgMusic = LoadMusicStream("assets/audio/music/dungeon_theme.mp3");
    PlayMusicStream(m_bgMusic);
    SetMusicVolume(m_bgMusic, 0.4f);
}

AudioSystem::~AudioSystem() {
    UnloadSound(m_sndCoin);
    UnloadSound(m_sndPickup);
    UnloadSound(m_sndPotion);
    UnloadSound(m_sndDoor);
    UnloadSound(m_sndLevelUp);
    UnloadSound(m_sndSword);

    for (auto& s : m_footsteps) UnloadSound(s);
    for (auto& s : m_hits) UnloadSound(s);

    UnloadMusicStream(m_bgMusic);
    CloseAudioDevice();
}

void AudioSystem::processEvent(GameEvent event) {
    switch (event) {
    case GameEvent::WALKED:
        playRandomFootstep();
        break;
    case GameEvent::ATTACKED:
        playRandomHit();
        break;
    case GameEvent::SWORD_ATTACK:
        PlaySound(m_sndSword);
        break;
    case GameEvent::PICKED_GOLD:
        PlaySound(m_sndCoin);
        break;
    case GameEvent::PICKED_LOOT:
        PlaySound(m_sndPickup);
        break;
    case GameEvent::UNLOCKED_DOOR:
        PlaySound(m_sndDoor);
        break;
    case GameEvent::LEVEL_UP:
        PlaySound(m_sndLevelUp);
        break;
    case GameEvent::DRINK_POTION:
        PlaySound(m_sndPotion);
        break;
    default: break;
    }
}

void AudioSystem::updateMusicStream() {
    UpdateMusicStream(m_bgMusic);
}

void AudioSystem::playRandomFootstep() {
    if (m_footsteps.empty()) return;

    int idx = std::rand() % m_footsteps.size();

    float randomPitch = 1.0f + (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * 0.30f;

    SetSoundPitch(m_footsteps[idx], randomPitch);
    PlaySound(m_footsteps[idx]);
}

void AudioSystem::playRandomHit() {
    if (m_hits.empty()) return;

    int idx = std::rand() % m_hits.size();

    float randomPitch = 0.90f + (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * 0.20f;

    SetSoundPitch(m_hits[idx], randomPitch);
    PlaySound(m_hits[idx]);
}