typedef enum IntroState {
	INTRO_STATE_START,
	INTRO_STATE_TYPING,
	INTRO_STATE_CONTINUE
} IntroState;

typedef struct IntroContext {
	f32 elapsed;
	IntroState state;
} IntroContext;

GLOBAL IntroContext intro_context;

PRIVATE void intro_set_state(IntroState state) {
	if (intro_context.state == state) {
		TraceLog(LOG_WARNING, "attempted to transition to the same state.");
		return;
	}

	// on exit
	intro_context.state = state;
	// on enter
	switch (intro_context.state) {
	case INTRO_STATE_TYPING: {
		PlaySound(resources_scroll_5_sound);
	} break;
	default: {
	} break;
	}
}

PUBLIC void intro_init(void) {
	intro_context = (IntroContext){
		.state = INTRO_STATE_START
	};
}

PUBLIC void intro_update(f32 delta) {
	switch (intro_context.state) {
	case INTRO_STATE_START: {
		intro_set_state(INTRO_STATE_TYPING);
	} break;
	case INTRO_STATE_TYPING: {
		intro_context.elapsed += delta;
		if (intro_context.elapsed >= 3.5f) {
			intro_set_state(INTRO_STATE_CONTINUE);
		}
	} break;
	case INTRO_STATE_CONTINUE: {
		if (IsKeyPressed(KEY_SPACE)) {
			PlaySound(resources_select_2_sound);
			scene_set_scene(SCENE_GAMEPLAY);
		} else if (IsKeyPressed(KEY_Q)) {
			quit_flagged = true;
		}
	} break;
	default: {
	} break;
	}
}

PUBLIC void intro_render(void) {
	const char *text = "The journey has just begun...";
	f32 text_font_size = resources_pixelplay_font.baseSize * 4.0f;
	f32 text_spacing = 3.0f;
	const char *subtext = TextSubtext(text, 0, intro_context.elapsed * 10.0f);
	Vector2 text_size = MeasureTextEx(resources_pixelplay_font, subtext, text_font_size, text_spacing);

	DrawTextEx(
		resources_pixelplay_font,
		subtext,
		(Vector2){
			GetScreenWidth() / 2.0f - text_size.x / 2.0f,
			GetScreenHeight() / 2.0f - text_size.y / 2.0f
		},
		text_font_size, text_spacing,
		THEME_BLACK
	);

	switch (intro_context.state) {
	case INTRO_STATE_CONTINUE: {
		ui_draw_space_confirm(THEME_BLACK);

		const char *title_text = "KOV";
		f32 title_text_font_size = resources_pixelplay_font.baseSize * 7.0f;
		Vector2 title_text_size = MeasureTextEx(resources_pixelplay_font, title_text, title_text_font_size, 4.0f);

		DrawTextEx(
			resources_pixelplay_font,
			title_text,
			(Vector2){
				GetScreenWidth() / 2.0f - title_text_size.x / 2.0f,
				GetScreenHeight() / 2.0f - 150.0f
			},
			title_text_font_size, 4.0f,
			THEME_BLACK
		);

		Vector2 q_size = { 32.0f, 32.0f };
		Vector2 quit_position = { 50.0f, GetScreenHeight() - q_size.y - 30.0f };
		Rectangle q_dst_rect = {
			quit_position.x - q_size.x / 2.0f,
			quit_position.y,
			q_size.x,
			q_size.y
		};
		DrawTexturePro(
			resources_prompt_texture,
			prompt_tiles[PROMPT_TILE_Q],
			q_dst_rect,
			Vector2Zero(), 0.0f,
			THEME_BLACK
		);
		const char *quit_text = "Quit";
		Vector2 quit_text_size = MeasureTextEx(resources_pixel_operator_font, quit_text, resources_pixel_operator_font.baseSize, 0.0f);
		DrawTextEx(
			resources_pixel_operator_font,
			quit_text,
			(Vector2){
				quit_position.x - quit_text_size.x / 2.0f,
				quit_position.y + q_size.y
			},
			resources_pixel_operator_font.baseSize, 0.0f,
			THEME_BLACK
		);
	} break;
	default:
		break;
	}
}
