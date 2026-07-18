#pragma once
#include <string>
#include <vector>

class Map {
public:
    void initialize(const std::string& layoutStr, int width, int height);

    char getTileAt(int x, int y) const;
    bool isPassable(int x, int y) const;

    int width() const { return m_width; }
    int height() const { return m_height; }

private:
    int m_width = 0;
    int m_height = 0;
    std::vector<std::string> m_grid;
};