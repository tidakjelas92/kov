#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

typedef enum GameScreen {
	GAME_SCREEN_LOGO,
	GAME_SCREEN_TITLE,
	GAME_SCREEN_GAMEPLAY,
	GAME_SCREEN_ENDING
} GameScreen;

static GameScreen game_screen;

int main(void) {
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "kov");
	SetTargetFPS(30);

	while (!WindowShouldClose()) {
		// update game
		switch (game_screen) {
		case GAME_SCREEN_LOGO: {
			if (GetTime() >= 3.0) {
				game_screen = GAME_SCREEN_TITLE;
			}
		} break;
		case GAME_SCREEN_TITLE: {
			if (IsKeyPressed(KEY_ENTER)) {
				game_screen = GAME_SCREEN_GAMEPLAY;
			}
		} break;
		case GAME_SCREEN_GAMEPLAY: {
			if (IsKeyPressed(KEY_ENTER)) {
				game_screen = GAME_SCREEN_ENDING;
			}
		} break;
		case GAME_SCREEN_ENDING: {
			if (IsKeyPressed(KEY_ENTER)) {
				game_screen = GAME_SCREEN_TITLE;
			}
		}
		}

		// render
		BeginDrawing();
		ClearBackground(GRAY);

		switch (game_screen) {
		case GAME_SCREEN_LOGO: {
			DrawText("Logo Screen", 20, 20, 40, LIGHTGRAY);
			const char *str = "Loading...";
			i32 str_width = MeasureText(str, 20);
			DrawText("Loading...", SCREEN_WIDTH / 2 - str_width / 2, SCREEN_HEIGHT / 2, 20, DARKGRAY);
		} break;
		case GAME_SCREEN_TITLE: {
			DrawText("Title Screen", 20, 20, 40, LIGHTGRAY);
			const char *str = "Press enter to start.";
			i32 str_width = MeasureText(str, 20);
			DrawText(str, SCREEN_WIDTH / 2 - str_width / 2, SCREEN_HEIGHT / 2, 20, DARKGRAY);
		} break;
		case GAME_SCREEN_GAMEPLAY: {
			DrawText("Gameplay Screen", 20, 20, 40, LIGHTGRAY);
			const char *str = "Press enter to end.";
			i32 str_width = MeasureText(str, 20);
			DrawText(str, SCREEN_WIDTH / 2 - str_width / 2, SCREEN_HEIGHT / 2, 20, DARKGRAY);
		} break;
		case GAME_SCREEN_ENDING: {
			DrawText("Ending Screen", 20, 20, 40, LIGHTGRAY);
			const char *str = "Press enter to go back to title.";
			i32 str_width = MeasureText(str, 20);
			DrawText(str, SCREEN_WIDTH / 2 - str_width / 2, SCREEN_HEIGHT / 2, 20, DARKGRAY);
		} break;
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
