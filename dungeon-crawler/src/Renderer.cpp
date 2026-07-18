#include "GameEngine.h"

Renderer::Renderer() {
    // Assets fall back safely if files are missing during development initialization
    Image img = LoadImage("assets/art/tileset.png");

    if (img.data == nullptr) {
        TraceLog(LOG_ERROR, "CRITICAL: Could not find or load 'assets/art/tileset.png'!");
    }

    m_tilemapTexture = LoadTextureFromImage(img);
    UnloadImage(img);
}

Renderer::~Renderer() {
    UnloadTexture(m_tilemapTexture);
}

void Renderer::drawTile(int screenX, int screenY, int texIdxX, int texIdxY) {
    Rectangle src = { (float)(texIdxX * TILE_SIZE), (float)(texIdxY * TILE_SIZE), (float)TILE_SIZE, (float)TILE_SIZE };
    Rectangle dest = { (float)screenX, (float)screenY, (float)SCALED_TILE_SIZE, (float)SCALED_TILE_SIZE };
    DrawTexturePro(m_tilemapTexture, src, dest, Vector2{ 0,0 }, 0.0f, WHITE);
}

void Renderer::draw(const Game& game) {
    ClearBackground(BLACK);

    // Calculate view bounds to render a viewport centered on the player
    int centerX = game.windowWidth() / 2;
    int centerY = game.windowHeight() / 2;

    int playerGridX = game.player().x;
    int playerGridY = game.player().y;

    // Dynamic Viewport Grid Loop 
    int tilesX = game.windowWidth() / SCALED_TILE_SIZE + 2;
    int tilesY = game.windowHeight() / SCALED_TILE_SIZE + 2;

    for (int viewY = -tilesY / 2; viewY <= tilesY / 2; ++viewY) {
        for (int viewX = -tilesX / 2; viewX <= tilesX / 2; ++viewX) {

            int mapX = playerGridX + viewX;
            int mapY = playerGridY + viewY;

            // Screen destination offset calculations
            int screenX = centerX + (viewX * SCALED_TILE_SIZE) - (SCALED_TILE_SIZE / 2);
            int screenY = centerY + (viewY * SCALED_TILE_SIZE) - (SCALED_TILE_SIZE / 2);

            char tile = game.map().getTileAt(mapX, mapY);

            // Layer 1: Render Map Ground Geometry
            if (tile == '#') {
                drawTile(screenX, screenY, 5, 5); // Tree/Wall block layout
            }
            else {
                drawTile(screenX, screenY, 4, 4); // Ground walk path floor tile
            }

            // Layer 2: Draw Active Non-Player Entities relative to player positions
            for (const auto& enemy : game.enemies()) {
                if (enemy.isAlive && enemy.x == mapX && enemy.y == mapY) {
                    drawTile(screenX, screenY, 11, 0); // Enemy orc vector sprite token
                }
            }
        }
    }

    // Layer 3: Render Player explicitly locked at the absolute focal center coordinates
    drawTile(centerX - (SCALED_TILE_SIZE / 2), centerY - (SCALED_TILE_SIZE / 2), 4, 0);

    // Layer 4: Modern User Interface HUD Data Text Overlays
    DrawRectangle(10, 10, 220, 65, Fade(SKYBLUE, 0.5f));
    DrawText(TextFormat("Player Pos: (%d, %d)", playerGridX, playerGridY), 20, 15, 16, WHITE);
    DrawText(TextFormat("Health: %d HP", game.player().health), 20, 35, 16, RED);
    DrawText(TextFormat("Gold Coins: $%d", game.player().money), 20, 55, 16, YELLOW);
}