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

    std::string mapStr = parser.get("Layout.map_string").value_or("");
    int mapW = std::stoi(parser.get("Layout.map_width").value_or("0"));
    int mapH = std::stoi(parser.get("Layout.map_height").value_or("0"));
    m_map.initialize(mapStr, mapW, mapH);

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

    try {
        parser.load("data/dungeon.json");
        TraceLog(LOG_INFO, "Parser loaded dungeon.json successfully!");

        if (auto p_hp = parser.get("player.hp")) m_player.health = std::stoi(*p_hp);
        if (auto p_attack = parser.get("player.attack")) m_player.baseAttack = std::stoi(*p_attack);
        if (auto p_coins = parser.get("player.coins")) m_player.coins = std::stoi(*p_coins);

        for (auto& enemy : m_enemies) {
            std::string keyPrefix = "enemies." + std::string(1, enemy.mapChar);

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

bool Game::isTileOccupiedByObstacle(int tx, int ty) const {
    char tile = m_map.getTileAt(tx, ty);
    if (tile == 'W' || tile == '#' || tile == 'D') return true;

    for (const auto& chest : m_chests) {
        if (chest.isAlive && chest.x == tx && chest.y == ty) return true;
    }

    for (const auto& genericEnemy : m_enemies) {
        if (genericEnemy.isAlive && genericEnemy.x == tx && genericEnemy.y == ty) return true;
    }

    if (m_player.x == tx && m_player.y == ty) return true;

    return false;
}

void Game::handleAction(Action a) {
    if (a == Action::NONE || a == Action::QUIT) return;

    bool playerTookAnActiveTurn = false;

    if (a == Action::USE_ITEM) {
        m_player.drinkPotion();
        playerTookAnActiveTurn = true;
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
            AudioBridge::trigger(GameEvent::UNLOCKED_DOOR);

            m_map.setTileAt(targetX, targetY, 'O');
            playerTookAnActiveTurn = true;
        }
        else {
            return;
        }
    }

    for (auto& enemy : m_enemies) {
        if (enemy.isAlive && enemy.x == targetX && enemy.y == targetY) {
            if (m_player.hasWeaponEquipped())
                AudioBridge::trigger(GameEvent::SWORD_ATTACK);
            
            enemy.takeDamage(m_player.getTotalAttack());
            playerTookAnActiveTurn = true;

            if (!enemy.isAlive) {
                m_player.gainExp(enemy.expReward);
                playerTookAnActiveTurn = true;

                Item droppedItem = Item::createDroppedLoot(enemy.x, enemy.y, enemy.getLootTypeEnum());
                m_chests.push_back(droppedItem);
            }
            else {
                m_player.takeDamage(enemy.attack);
            }
            return;
        }
    }

    for (auto& item : m_chests) {
        if (item.isAlive && item.x == targetX && item.y == targetY) {
            m_player.collectItem(item);
            item.isAlive = false;

            playerTookAnActiveTurn = true;

            if (item.type == ItemType::CHEST) {
                m_map.setTileAt(targetX, targetY, 'V');
            }
        }
    }

    if (m_map.isPassable(targetX, targetY)) {
        if (m_player.x != targetX || m_player.y != targetY) {
            AudioBridge::trigger(GameEvent::WALKED);
            playerTookAnActiveTurn = true;
        }

        int deltaX = targetX - m_player.x;
        int deltaY = targetY - m_player.y;

        m_player.move(deltaX, deltaY);
    }

    if (playerTookAnActiveTurn)
        m_enemyTurnPending = true;
}

void Game::executeEnemyTurn() {
    m_enemyTurnPending = false;

    for (auto& enemy : m_enemies) {
        if (!enemy.isAlive) continue;

        int currentDistance = std::abs(enemy.x - m_player.x) + std::abs(enemy.y - m_player.y);

        if (currentDistance == 1) {
            m_player.takeDamage(enemy.attack);
            AudioBridge::trigger(GameEvent::ATTACKED);

            continue;
        }

        int nextEnemyX = enemy.x;
        int nextEnemyY = enemy.y;

        enemy.updateAI(m_player.x, m_player.y, nextEnemyX, nextEnemyY);

        if (!isTileOccupiedByObstacle(nextEnemyX, nextEnemyY)) {
            enemy.x = nextEnemyX;
            enemy.y = nextEnemyY;
        }
    }
}

Action Game::mapKeyToAction(int pressedKey) const {
    auto it = m_keyBindings.find(pressedKey);
    return (it != m_keyBindings.end()) ? it->second : Action::NONE;
}

void Game::restart() {
    m_player.health = m_player.maxHealth;
    m_player.currentExp = 0;
    m_player.level = 1;
    m_player.coins = 0;
    m_player.equippedWeaponName = "Fists";
    m_player.m_equippedWeapon = nullptr;

    for (Item* ptr : m_player.m_inventory) {
        delete ptr;
    }
    m_player.m_inventory.clear();

    m_enemies.clear();
    m_chests.clear();

    this->load("data/dungeon.ini");
}