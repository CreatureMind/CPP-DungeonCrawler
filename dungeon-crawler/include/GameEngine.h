#pragma once
#include "raylib.h"
#include "raymath.h"
#include <string>
#include <vector>
#include <map>

constexpr int TILE_SIZE = 8;
constexpr int TILE_SCALE = 3;
constexpr int SCALED_TILE_SIZE = TILE_SIZE * TILE_SCALE;

constexpr float AI_TURN_DELAY = 0.20f;

#include "GameEvents.h"
#include "AudioSystem.h"
#include "AudioBridge.h"
#include "Action.h"
#include "InputHandler.h"
#include "Item.h"
#include "Player.h"
#include "Enemy.h"
#include "Map.h"

#include "Game.h"
#include "Renderer.h"