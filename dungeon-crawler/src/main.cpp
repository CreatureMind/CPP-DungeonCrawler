#include "GameEngine.h"
#include <ctime>
#include <cstdlib>

int main(int argc, char** argv) {
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	Game game;
	game.load("data/dungeon.ini");

	InitWindow(game.windowWidth(),
		game.windowHeight(),
		game.title().c_str());

	SetTargetFPS(60);

	AudioSystem audio;
	Renderer renderer;

	AudioBridge::registerSystem([&audio](GameEvent e) {
		audio.processEvent(e);
	});

	float aiTimer = 0.0f;

	while (!WindowShouldClose())
	{
		audio.updateMusicStream();

		if (!game.isGameOver() && !game.isGameWon()) {
			if (!game.isEnemyTurn()) {
				Action a = pollAction(game);
				if (a == Action::QUIT) break;
				game.handleAction(a);
			}
			else {
				aiTimer += GetFrameTime();
				if (aiTimer >= AI_TURN_DELAY) {
					game.executeEnemyTurn();
					aiTimer = 0.0f;
				}
			}
		}
		else {
			if (IsKeyPressed(KEY_R)) {
				game.restart();
				aiTimer = 0.0f;
			}
		}

		BeginDrawing();
		renderer.draw(game);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}