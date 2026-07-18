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

	Renderer renderer;
	while (!WindowShouldClose())
	{
		Action action = pollAction(game);

		if (action == Action::QUIT) break;

		game.handleAction(action);

		BeginDrawing();
		renderer.draw(game);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}