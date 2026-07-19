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
    bind("Controls.USE_ITEM", Action::USE_ITEM);
    bind("Controls.QUIT", Action::QUIT);

    // Map parsing
    std::string mapStr = parser.get("Layout.map_string").value_or("");
    int mapW = std::stoi(parser.get("Layout.map_width").value_or("0"));
    int mapH = std::stoi(parser.get("Layout.map_height").value_or("0"));
    m_map.initialize(mapStr, mapW, mapH);

    // Scan map for actor spawn triggers
    m_chests.clear();
    for (int y = 0; y < mapH; ++y) {
        for (int x = 0; x < mapW; ++x) {
            char tile = m_map.getTileAt(x, y);

            if (tile == '@') {
                m_player.x = x;
                m_player.y = y;
            }
            else if (tile == 'G' || tile == 'S' || tile == 'E') {
                Enemy e;
                e.x = x;
                e.y = y;
                e.mapChar = tile;
                m_enemies.push_back(e);
            }
            else if (tile == 'C') {
                Item chest = Item::createDroppedLoot(x, y, ItemType::CHEST);
                m_chests.push_back(chest);
            }
        }
    }

    // Load custom configuration JSON layout values
    try {
        parser.load("data/dungeon.json");
        TraceLog(LOG_INFO, "Parser loaded dungeon.json successfully!");

        if (auto p_hp = parser.get("player.hp")) m_player.health = std::stoi(*p_hp);
        if (auto p_attack = parser.get("player.attack")) m_player.baseAttack = std::stoi(*p_attack);
        if (auto p_coins = parser.get("player.coins")) m_player.coins = std::stoi(*p_coins);

        for (auto& enemy : m_enemies) {
            std::string keyPrefix = "enemies." + std::string(1, enemy.mapChar);

            // Extract stats from JSON based on the character token box layout
            if (auto e_name = parser.get(keyPrefix + ".name"))   enemy.name = *e_name;
            if (auto e_hp = parser.get(keyPrefix + ".hp")) enemy.health = std::stoi(*e_hp);
            if (auto e_attack = parser.get(keyPrefix + ".attack")) enemy.attack = std::stoi(*e_attack);
            if (auto e_tileX = parser.get(keyPrefix + ".tileX"))  enemy.tileX = std::stoi(*e_tileX);
            if (auto e_tileY = parser.get(keyPrefix + ".tileY"))  enemy.tileY = std::stoi(*e_tileY);
            if (auto e_exp = parser.get(keyPrefix + ".exp"))  enemy.expReward = std::stoi(*e_exp);
            if (auto e_loot = parser.get(keyPrefix + ".loot")) {
                enemy.lootDropType = *e_loot;
            }
            else {
                enemy.lootDropType = "random";
            }
        }
    }
    catch (const std::exception& e) {
        TraceLog(LOG_ERROR, "JSON Parsing Failure Error: %s", e.what());
    }
}

void Game::handleAction(Action a) {
    if (a == Action::NONE || a == Action::QUIT) return;

    if (a == Action::USE_ITEM) {
        m_player.drinkPotion();
        return;
    }

    int targetX = m_player.x;
    int targetY = m_player.y;

    if (a == Action::MOVE_UP)    targetY -= 1;
    if (a == Action::MOVE_DOWN)  targetY += 1;
    if (a == Action::MOVE_LEFT)  targetX -= 1;
    if (a == Action::MOVE_RIGHT) targetX += 1;

    char targetTile = m_map.getTileAt(targetX, targetY);
    if (targetTile == 'D') {
        if (m_player.useKey()) {
            m_map.setTileAt(targetX, targetY, 'O');
        }
        else {
            return;
        }
    }

    for (auto& enemy : m_enemies) {
        if (enemy.isAlive && enemy.x == targetX && enemy.y == targetY) {
            enemy.takeDamage(m_player.getTotalAttack());

            if (!enemy.isAlive) {
                m_player.gainExp(enemy.expReward);
                Item droppedItem = Item::createDroppedLoot(enemy.x, enemy.y, enemy.getLootTypeEnum());
                m_chests.push_back(droppedItem);
            }
            else {
                m_player.takeDamage(enemy.attack);
            }
            return;
        }
    }

    // 2. Check for item picking options
    for (auto& item : m_chests) {
        if (item.isAlive && item.x == targetX && item.y == targetY) {
            m_player.collectItem(item);
            item.isAlive = false;

            if (item.type == ItemType::CHEST) {
                m_map.setTileAt(targetX, targetY, '.');
            }
        }
    }

    // 3. Process movement if path tile is safe and accessible
    if (m_map.isPassable(targetX, targetY)) {
        int deltaX = targetX - m_player.x;
        int deltaY = targetY - m_player.y;
        m_player.move(deltaX, deltaY);
    }
}

Action Game::mapKeyToAction(int pressedKey) const {
    auto it = m_keyBindings.find(pressedKey);
    return (it != m_keyBindings.end()) ? it->second : Action::NONE;
}