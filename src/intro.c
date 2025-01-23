typedef enum IntroState {
	INTRO_STATE_TYPING,
	INTRO_STATE_CONTINUE
} IntroState;

typedef struct IntroContext {
	const char *intro_text;
	f32 intro_text_font_size;
	f32 intro_text_spacing;
	f32 elapsed;
	IntroState state;
} IntroContext;

GLOBAL IntroContext intro_context;

PUBLIC void intro_init(void) {
	intro_context = (IntroContext){
		.intro_text = "The journey has just begun...",
		.intro_text_font_size = resources_pixelplay_font.baseSize * 4.0f,
		.intro_text_spacing = 3.0f,
	};
}

PUBLIC void intro_update(f32 delta) {
	intro_context.elapsed += delta;
	if (intro_context.elapsed >= 5.0f) {
		scene_set_scene(SCENE_GAMEPLAY);
	}
}

PUBLIC void intro_render(void) {
	Vector2 intro_text_size = MeasureTextEx(
		resources_pixelplay_font,
		intro_context.intro_text,
		intro_context.intro_text_font_size,
		intro_context.intro_text_spacing
	);
	DrawTextEx(
		resources_pixelplay_font,
		TextSubtext(intro_context.intro_text, 0, intro_context.elapsed * 10.0f),
		(Vector2){
			GetScreenWidth() / 2.0f - intro_text_size.x / 2.0f,
			GetScreenHeight() / 2.0f - intro_text_size.y / 2.0f
		},
		intro_context.intro_text_font_size, intro_context.intro_text_spacing,
		(Color){ 110, 100, 90, 255 }
	);
}
