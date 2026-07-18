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
    // 1. Guard check: Ensure 'y' is a valid row index in our grid vector
    if (y < 0 || y >= static_cast<int>(m_grid.size())) {
        return '#'; // Safely return a wall if out of vertical bounds
    }

    // 2. Guard check: Ensure 'x' is a valid column index inside that specific row string
    if (x < 0 || x >= static_cast<int>(m_grid[y].size())) {
        return '#'; // Safely return a wall if out of horizontal bounds
    }

    // 3. If both bounds pass safely, return the actual layout character token
    return m_grid[y][x];
}

bool Map::isPassable(int x, int y) const {
    char tile = getTileAt(x, y);
    return (tile != '#'); // Walls block movement
}