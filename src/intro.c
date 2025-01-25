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
			scene_set_scene(SCENE_TRAINING);
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
		ui_draw_space_confirm();
	} break;
	default:
		break;
	}
}
