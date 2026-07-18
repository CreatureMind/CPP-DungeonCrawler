#pragma once
#include <map>
#include <string>
#include <vector>
#include "Map.h"
#include "Player.h"
#include "Enemy.h"
#include "Action.h"

struct Player;
struct Enemy;
struct Item;
class Map;

class Game {
public:
    void load(const std::string& iniFilepath);
    void handleAction(Action a);

    int windowWidth() const { return m_windowWidth; }
    int windowHeight() const { return m_windowHeight; }
    const std::string& title() const { return m_title; }
    Action mapKeyToAction(int pressedKey) const;

    // Read-only getters for the Renderer
    const Player& player() const { return m_player; }
    const Map& map() const { return m_map; }
    const std::vector<Enemy>& enemies() const { return m_enemies; }
    const std::vector<Item>& chests() const { return m_chests; }

private:
    int m_windowWidth = 800;
    int m_windowHeight = 600;
    std::string m_title = "C++ Dungeon Crawler";

    Player m_player;
    Map m_map;
    std::vector<Enemy> m_enemies;
    std::vector<Item> m_chests;
    std::map<int, Action> m_keyBindings;
};