#include "GameEngine.h"

void Map::initialize(const std::string& layoutStr, int width, int height) {
    m_width = width;
    m_height = height;
    m_grid.clear();

    if (layoutStr.empty() || width <= 0) return;

    for (int y = 0; y < height; ++y) {
        if (y * width < (int)layoutStr.size()) {
            m_grid.push_back(layoutStr.substr(y * width, width));
        }
    }
}

char Map::getTileAt(int x, int y) const {
    if (y < 0 || y >= static_cast<int>(m_grid.size())) {
        return '#';
    }

    if (x < 0 || x >= static_cast<int>(m_grid[y].size())) {
        return '#';
    }

    return m_grid[y][x];
}

void Map::setTileAt(int x, int y, char newTile) {
    if (y >= 0 && y < static_cast<int>(m_grid.size())) {
        if (x >= 0 && x < static_cast<int>(m_grid[y].size())) {
            m_grid[y][x] = newTile;
        }
    }
}

bool Map::isPassable(int x, int y) const {
    char tile = getTileAt(x, y);
    return (tile != '#' && tile != 'W');
}