#pragma once
#include "Game.h"

class Renderer {
public:
    Renderer();
    ~Renderer();
    void draw(const Game& game);

private:
    Texture2D m_tilemapTexture;
    Font m_font;
    void drawTile(int screenX, int screenY, int texIdxX, int texIdxY);
};