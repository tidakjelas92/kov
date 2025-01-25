typedef enum EndingState {
	ENDING_STATE_START,
	ENDING_STATE_TYPING,
	ENDING_STATE_CONTINUE
} EndingState;

typedef struct EndingContext {
	f32 elapsed;
	EndingState state;
} EndingContext;

GLOBAL EndingContext ending_context;

PRIVATE void ending_set_state(EndingState state) {
	if (ending_context.state == state) {
		TraceLog(LOG_WARNING, "attempted to transition to the same state.");
		return;
	}

	// on exit
	ending_context.state = state;
	// on enter
	switch (ending_context.state) {
	case ENDING_STATE_TYPING: {
		PlaySound(resources_scroll_5_sound);
	} break;
	default: {
	} break;
	}
}

PUBLIC void ending_init(void) {
	ending_context = (EndingContext){
		.state = ENDING_STATE_START
	};
}

PUBLIC void ending_update(f32 delta) {
	switch (ending_context.state) {
	case ENDING_STATE_START: {
		ending_set_state(ENDING_STATE_TYPING);
	} break;
	case ENDING_STATE_TYPING: {
		ending_context.elapsed += delta;
		if (ending_context.elapsed >= 3.5f) {
			ending_set_state(ENDING_STATE_CONTINUE);
		}
	} break;
	case ENDING_STATE_CONTINUE: {
		if (IsKeyPressed(KEY_SPACE)) {
			PlaySound(resources_select_2_sound);
			scene_set_scene(SCENE_INTRO);
		}
	} break;
	default: {
	} break;
	}
}

PUBLIC void ending_render(void) {
	const char *text = "Congratulations";
	f32 text_font_size = resources_pixelplay_font.baseSize * 4.0f;
	f32 text_spacing = 3.0f;
	const char *subtext = TextSubtext(text, 0, ending_context.elapsed * 10.0f);
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

	switch (ending_context.state) {
	case ENDING_STATE_CONTINUE: {
		ui_draw_space_confirm(THEME_BLACK);
	} break;
	default:
		break;
	}
}
