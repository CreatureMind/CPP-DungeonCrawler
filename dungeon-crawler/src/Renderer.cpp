#include "GameEngine.h"


const float startX = 15.0f;
const float startY = 15.0f;
const float fontSize = 16.0f;
const float lineSpacing = 20.0f;
const float padding = 10.0f;

float maxWidth = 0.0f;
float totalHeight = 0.0f;

Renderer::Renderer() {
    // Assets fall back safely if files are missing during development initialization
    Image img = LoadImage("assets/art/tileset.png");

    if (img.data == nullptr) {
        TraceLog(LOG_ERROR, "CRITICAL: Could not find or load 'assets/art/tileset.png'!");
    }

    m_tilemapTexture = LoadTextureFromImage(img);
    m_font = LoadFont("assets/fonts/8-bit-hud.ttf");
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
            else if (tile == 'D') {
                drawTile(screenX, screenY, 4, 2);
            }
            else if (tile == 'O') {
                drawTile(screenX, screenY, 5, 2);
            }
            else {
                drawTile(screenX, screenY, 4, 4); // Ground walk path floor tile
            }

            for (const auto& item : game.chests()) {
                if (item.isAlive && item.x == mapX && item.y == mapY) {
                    drawTile(screenX, screenY, item.tileX, item.tileY);
                }
            }

            // Layer 2: Draw Active Non-Player Entities relative to player positions
            for (const auto& enemy : game.enemies()) {
                if (enemy.isAlive && enemy.x == mapX && enemy.y == mapY) {
                    drawTile(screenX, screenY, enemy.tileX, enemy.tileY);
                }
            }
        }
    }

    // Layer 3: Render Player explicitly locked at the absolute focal center coordinates
    drawTile(centerX - (SCALED_TILE_SIZE / 2), centerY - (SCALED_TILE_SIZE / 2), 4, 0);

    // Layer 4: Modern User Interface HUD Data Text Overlays
    std::vector<HudLine> hudLines = {
        { TextFormat("LVL %u", game.player().level), WHITE },
        { TextFormat("Health Points: %u", game.player().health), RED },
        { TextFormat("Experience Points: %d", game.player().currentExp), GREEN },
        { TextFormat("Gold Coins: %d", game.player().coins), GOLD },
        { TextFormat("Equipped: %s", game.player().equippedWeaponName.c_str()), GRAY },
        { TextFormat("Total Attack: %d ATK", game.player().getTotalAttack()), PURPLE },
        { TextFormat("Potions: %d", game.player().potionCount), BLUE },
        { TextFormat("Keys: %d", game.player().keyCount), YELLOW }
    };

    for (size_t i = 0; i < hudLines.size(); ++i) {
        // Measure the exact size of this line using Raylib's text engine
        Vector2 textSize = MeasureTextEx(m_font, hudLines[i].text.c_str(), fontSize, 0.0f);

        // Find the longest line to dictate the final width
        if (textSize.x > maxWidth) {
            maxWidth = textSize.x;
        }
    }
    totalHeight = (hudLines.size() - 1) * lineSpacing + fontSize;

    float boxX = startX - padding;
    float boxY = startY - padding;
    float boxWidth = maxWidth + (padding * 2);
    float boxHeight = totalHeight + (padding * 2);

    DrawRectangleRec(Rectangle{ boxX, boxY, boxWidth, boxHeight }, Fade(BLACK, 0.8f));

    for (size_t i = 0; i < hudLines.size(); ++i) {
        float currentY = startY + (i * lineSpacing);
        DrawTextEx(m_font, hudLines[i].text.c_str(), Vector2{ startX, currentY }, fontSize, 0.0f, hudLines[i].color);
    }
}