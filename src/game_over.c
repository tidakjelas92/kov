typedef enum GameOverState {
	GAME_OVER_STATE_START,
	GAME_OVER_STATE_WAIT,
	GAME_OVER_STATE_CONTINUE
} GameOverState;

typedef struct GameOverContext {
	f32 elapsed;
	GameOverState state;
} GameOverContext;

GLOBAL GameOverContext game_over_context;

PRIVATE void game_over_set_state(GameOverState state) {
	if (game_over_context.state == state) {
		TraceLog(LOG_WARNING, "attempted to transition to the same state.");
		return;
	}

	// on exit
	game_over_context.state = state;
	// on enter
	switch (game_over_context.state) {
	case GAME_OVER_STATE_WAIT: {
		game_over_context.elapsed = 0.0f;
	} break;
	default: {
	} break;
	}
}

PUBLIC void game_over_init(void) {
	game_over_context = (GameOverContext){
		.state = GAME_OVER_STATE_START
	};
}

PUBLIC void game_over_update(f32 delta) {
	switch (game_over_context.state) {
	case GAME_OVER_STATE_START: {
		game_over_set_state(GAME_OVER_STATE_WAIT);
	} break;
	case GAME_OVER_STATE_WAIT: {
		game_over_context.elapsed += delta;
		if (game_over_context.elapsed >= 1.5f) {
			game_over_set_state(GAME_OVER_STATE_CONTINUE);
		}
	} break;
	case GAME_OVER_STATE_CONTINUE: {
		if (IsKeyPressed(KEY_SPACE)) {
			scene_set_scene(SCENE_INTRO);
		}
	} break;
	default: {
	} break;
	}
}

PUBLIC void game_over_render(void) {
	DrawRectangle(15, 15, GetScreenWidth() - 30, GetScreenHeight() - 30, THEME_BLACK);

	const char *text = "GAME OVER";
	f32 text_font_size = resources_pixelplay_font.baseSize * 4.0f;
	f32 text_spacing = 3.0f;
	Vector2 text_size = MeasureTextEx(resources_pixelplay_font, text, text_font_size, text_spacing);

	DrawTextEx(
		resources_pixelplay_font,
		text,
		(Vector2){
			GetScreenWidth() / 2.0f - text_size.x / 2.0f,
			GetScreenHeight() / 2.0f - text_size.y / 2.0f
		},
		text_font_size, text_spacing,
		THEME_WHITE
	);

	switch (game_over_context.state) {
	case GAME_OVER_STATE_CONTINUE: {
		ui_draw_space_confirm(THEME_WHITE);
	} break;
	default:
		break;
	}
}
