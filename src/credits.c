PUBLIC void credits_update(void) {
	if (IsKeyPressed(KEY_SPACE)) {
		scene_set_scene(SCENE_INTRO);
	}
}

PUBLIC void credits_render(void) {
	const char *title_text = "CREDITS";
	f32 title_text_font_size = resources_pixelplay_font.baseSize * 7.0f;
	Vector2 title_text_size = MeasureTextEx(resources_pixelplay_font, title_text, title_text_font_size, 4.0f);
	Vector2 title_position = {
		GetScreenWidth() / 2.0f - title_text_size.x / 2.0f,
		GetScreenHeight() / 2.0f - 225.0f
	};

	DrawTextEx(
		resources_pixelplay_font, title_text, title_position, title_text_font_size, 4.0f, THEME_BLACK
	);

	const char *texts[] = {
		"raylib",
		"Kenney's 1-bit input prompts",
		"Kenney's Interface Sounds",
		"Kenney's Monochrome RPG",
		"Pixelplay Font",
		"Pixel Operator Font"
	};
	f32 text_font_size = resources_pixel_operator_font.baseSize * 2.0f;
	f32 position = title_position.y + title_text_size.y;

	for (u32 i = 0; i < sizeof(texts) / sizeof(const char *); i++) {
		Vector2 text_size = MeasureTextEx(resources_pixel_operator_font, texts[i], text_font_size, 0.0f);
		DrawTextEx(
			resources_pixel_operator_font,
			texts[i],
			(Vector2){ GetScreenWidth() / 2.0f - text_size.x / 2.0f, position },
			text_font_size, 0.0f,
			THEME_BLACK
		);
		position += text_size.y;
	}

	ui_draw_space_confirm(THEME_BLACK);
}
