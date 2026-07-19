#include "GameEngine.h"

const float startX = 15.0f;
const float startY = 15.0f;
const float fontSize = 16.0f;
const float lineSpacing = 20.0f;
const float padding = 10.0f;

float maxWidth = 0.0f;
float totalHeight = 0.0f;

Renderer::Renderer() {
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
    UnloadFont(m_font);
}

void Renderer::drawTile(int screenX, int screenY, int texIdxX, int texIdxY) {
    Rectangle src = { (float)(texIdxX * TILE_SIZE), (float)(texIdxY * TILE_SIZE), (float)TILE_SIZE, (float)TILE_SIZE };
    Rectangle dest = { (float)screenX, (float)screenY, (float)SCALED_TILE_SIZE, (float)SCALED_TILE_SIZE };
    DrawTexturePro(m_tilemapTexture, src, dest, Vector2{ 0,0 }, 0.0f, WHITE);
}

void Renderer::draw(const Game& game) {
    ClearBackground(BLACK);

    int centerX = game.windowWidth() / 2;
    int centerY = game.windowHeight() / 2;
    int playerGridX = game.player().x;
    int playerGridY = game.player().y;

    int tilesX = game.windowWidth() / SCALED_TILE_SIZE + 2;
    int tilesY = game.windowHeight() / SCALED_TILE_SIZE + 2;

    for (int viewY = -tilesY / 2; viewY <= tilesY / 2; ++viewY) {
        for (int viewX = -tilesX / 2; viewX <= tilesX / 2; ++viewX) {

            int mapX = playerGridX + viewX;
            int mapY = playerGridY + viewY;

            int screenX = centerX + (viewX * SCALED_TILE_SIZE) - (SCALED_TILE_SIZE / 2);
            int screenY = centerY + (viewY * SCALED_TILE_SIZE) - (SCALED_TILE_SIZE / 2);

            char tile = game.map().getTileAt(mapX, mapY);

            if (tile == '#') {
                drawTile(screenX, screenY, 5, 5); // Tree/Wall block layout
            }
            else if (tile == 'D') {
                drawTile(screenX, screenY, 4, 2); // Door
            }
            else if (tile == 'O') {
                drawTile(screenX, screenY, 5, 2); // Opened Door
            }
            else if (tile == 'V') {
                drawTile(screenX, screenY, 10, 3); // Opened Chest
            }
            else if (tile == 'W') {
                drawTile(screenX, screenY, 1, 0); // Wall
            }
            else {
                drawTile(screenX, screenY, 4, 4); // Ground walk path floor tile
            }

            for (const auto& item : game.chests()) {
                if (item.isAlive && item.x == mapX && item.y == mapY) {
                    drawTile(screenX, screenY, item.tileX, item.tileY);
                }
            }

            for (const auto& enemy : game.enemies()) {
                if (enemy.isAlive && enemy.x == mapX && enemy.y == mapY) {
                    drawTile(screenX, screenY, enemy.tileX, enemy.tileY);
                }
            }
        }
    }

    //Rendering Player at the center
    drawTile(centerX - (SCALED_TILE_SIZE / 2), centerY - (SCALED_TILE_SIZE / 2), 4, 0);

    if (game.isGameOver()) {
        DrawRectangle(0, 0, game.windowWidth(), game.windowHeight(), Fade(BLACK, 0.6f));
    }
    else if (game.isGameWon()) {
        DrawRectangle(0, 0, game.windowWidth(), game.windowHeight(), Fade(GOLD, 0.3f));
    }

    std::vector<HudLine> linesToDraw;
    Color boxColor = Fade(BLACK, 0.8f);
    Color borderColor = BLANK;

    if (!game.isGameOver() && !game.isGameWon()) {
        linesToDraw = {
            { TextFormat("LVL %u", game.player().level), WHITE },
            { TextFormat("Health Points: %u", game.player().health), RED },
            { TextFormat("Experience Points: %d", game.player().currentExp), GREEN },
            { TextFormat("Gold Coins: %d", game.player().coins), GOLD },
            { TextFormat("Equipped: %s", game.player().equippedWeaponName.c_str()), GRAY },
            { TextFormat("Total Attack: %d ATK", game.player().getTotalAttack()), PURPLE },
            { TextFormat("Potions: %d", game.player().potionCount), BLUE },
            { TextFormat("Keys: %d", game.player().keyCount), YELLOW }
        };
    }
    else if (game.isGameOver()) {
        linesToDraw = {
            { "GAME OVER", RED },
            { "Press 'R' to Restart", WHITE },
            { "Thank you for playing!", LIGHTGRAY }
        };
        borderColor = RED;
    }
    else if (game.isGameWon()) {
        linesToDraw = {
            { "VICTORY ACHIEVED!", GOLD },
            { TextFormat("Final Level: %u", game.player().level), WHITE },
            { TextFormat("Total Gold Earned: $%d", game.player().coins), GOLD },
            { "Press 'R' to Play Again", WHITE },
            { "Thank you for playing!", LIGHTGRAY }
        };
        borderColor = GOLD;
    }

    maxWidth = 0.0f;

    for (const auto& line : linesToDraw) {
        Vector2 size = MeasureTextEx(m_font, line.text.c_str(), fontSize, 0.0f);
        if (size.x > maxWidth) maxWidth = size.x;
    }
    totalHeight = (linesToDraw.size() - 1) * lineSpacing + fontSize;

    float boxWidth = maxWidth + (padding * 2);
    float boxHeight = totalHeight + (padding * 2);
    float boxX, boxY;

    if (!game.isGameOver() && !game.isGameWon()) {
        boxX = startX - padding;
        boxY = startY - padding;
    }
    else {
        boxX = (game.windowWidth() - boxWidth) / 2.0f;
        boxY = (game.windowHeight() - boxHeight) / 2.0f;
    }

    DrawRectangleRec(Rectangle{ boxX, boxY, boxWidth, boxHeight }, boxColor);
    if (borderColor.a > 0) {
        DrawRectangleLinesEx(Rectangle{ boxX, boxY, boxWidth, boxHeight }, 2, borderColor);
    }

    float startTextX = boxX + padding;
    float startTextY = boxY + padding;

    for (size_t i = 0; i < linesToDraw.size(); ++i) {
        float currentY = startTextY + (i * lineSpacing);

        float textX = startTextX;
        if (game.isGameOver() || game.isGameWon()) {
            Vector2 size = MeasureTextEx(m_font, linesToDraw[i].text.c_str(), fontSize, 0.0f);
            textX = boxX + (boxWidth - size.x) / 2.0f;
        }

        DrawTextEx(m_font, linesToDraw[i].text.c_str(), Vector2{ textX, currentY }, fontSize, 0.0f, linesToDraw[i].color);
    }
}