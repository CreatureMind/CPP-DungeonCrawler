#include "GameEngine.h"
#include "config/ConfigManager.h"
#include <cctype>

void Game::load(const std::string& iniFilepath) {
    ConfigManager parser;
    try {
        parser.load(iniFilepath);
    }
    catch (const std::exception& e) {
        TraceLog(LOG_ERROR, "Failed to load dynamic file path: %s", iniFilepath.c_str());
        return;
    }

    if (auto w = parser.get("Window.width"))  m_windowWidth = std::stoi(*w);
    if (auto h = parser.get("Window.height")) m_windowHeight = std::stoi(*h);
    if (auto t = parser.get("Window.title"))  m_title = *t;

    // Controls Mapper
    auto bind = [&](const std::string& sectionAndKey, Action act) {
        if (auto val = parser.get(sectionAndKey)) {
            if (!val->empty()) {
                char firstLetter = (*val)[0];
                int raylibKeyCode = std::toupper(static_cast<unsigned char>(firstLetter));
                m_keyBindings[raylibKeyCode] = act;

                TraceLog(LOG_INFO, "Mapped control key '%c' (Code %d) to action!", firstLetter, raylibKeyCode);
            }
        }
    };

    bind("Controls.UP", Action::MOVE_UP);
    bind("Controls.DOWN", Action::MOVE_DOWN);
    bind("Controls.LEFT", Action::MOVE_LEFT);
    bind("Controls.RIGHT", Action::MOVE_RIGHT);

    // Map parsing
    std::string mapStr = parser.get("Layout.map_string").value_or("");
    int mapW = std::stoi(parser.get("Layout.map_width").value_or("0"));
    int mapH = std::stoi(parser.get("Layout.map_height").value_or("0"));
    m_map.initialize(mapStr, mapW, mapH);

    // Scan map for actor spawn triggers
    for (int y = 0; y < mapH; ++y) {
        for (int x = 0; x < mapW; ++x) {
            char tile = m_map.getTileAt(x, y);
            if (tile == '@') {
                m_player.x = x; m_player.y = y;
            }
            else if (tile == 'M') {
                Enemy e; e.x = x; e.y = y; m_enemies.push_back(e);
            }
        }
    }

    // Load custom configuration JSON layout values
    try {
        parser.load("data/dungeon.json");
        if (auto p_hp = parser.get("player.hp")) m_player.health = std::stoi(*p_hp);
        if (auto p_at = parser.get("player.attack")) m_player.attack = std::stoi(*p_at);
    }
    catch (...) {}
}

void Game::handleAction(Action a) {
    if (a == Action::NONE || a == Action::QUIT) return;

    int targetX = m_player.x;
    int targetY = m_player.y;

    if (a == Action::MOVE_UP)    targetY -= 1;
    if (a == Action::MOVE_DOWN)  targetY += 1;
    if (a == Action::MOVE_LEFT)  targetX -= 1;
    if (a == Action::MOVE_RIGHT) targetX += 1;

    // 1. Check for enemy interactions
    for (auto& enemy : m_enemies) {
        if (enemy.isAlive && enemy.x == targetX && enemy.y == targetY) {
            resolveAttack(m_player, enemy);
            return; // Exit early to prevent walking on top of living enemies
        }
    }

    // 2. Check for item picking options
    for (auto it = m_chests.begin(); it != m_chests.end(); ++it) {
        if (it->isAlive && it->x == targetX && it->y == targetY) {
            m_player.money += it->money;
            it->isAlive = false;
        }
    }

    // 3. Process movement if path tile is safe and accessible
    if (m_map.isPassable(targetX, targetY)) {
        m_player.x = targetX;
        m_player.y = targetY;
    }
}

Action Game::mapKeyToAction(int pressedKey) const {
    auto it = m_keyBindings.find(pressedKey);
    return (it != m_keyBindings.end()) ? it->second : Action::NONE;
}