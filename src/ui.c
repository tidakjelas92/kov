PUBLIC void ui_draw_space_confirm(void) {
	Rectangle space_src_rect = { 496.0f, 224.0f, 48.0f, 16.0f };
	Vector2 space_dst_size = { 96.0f, 32.0f };
	f32 text_spacing = 3.0f;
	Rectangle space_dst_rect = {
		GetScreenWidth() / 2.0f - space_dst_size.x / 2.0f,
		GetScreenHeight() - space_dst_size.y - 30.0f,
		space_dst_size.x,
		space_dst_size.y
	};
	DrawTexturePro(
		resources_prompt_texture,
		space_src_rect,
		space_dst_rect,
		Vector2Zero(), 0.0f,
		THEME_BLACK
	);

	const char *continue_text = "CONFIRM";
	Vector2 continue_text_size = MeasureTextEx(resources_pixelplay_font, continue_text, resources_pixelplay_font.baseSize, text_spacing);
	DrawTextEx(
		resources_pixelplay_font,
		continue_text,
		(Vector2){
			GetScreenWidth() / 2.0f - continue_text_size.x / 2.0f,
			space_dst_rect.y - continue_text_size.y
		},
		resources_pixelplay_font.baseSize, text_spacing,
		THEME_BLACK
	);
}


// position is center position
// value is normalized.
PUBLIC void ui_draw_health_bar(Vector2 position, f32 width, f32 value) {
	Vector2 start_position = { position.x - width / 2.0f, position.y - 10.0f };
	DrawRectangleRec(
		(Rectangle){ start_position.x, start_position.y, width, 20.0f },
		THEME_BLACK
	);
	DrawRectangleRec(
		(Rectangle){ start_position.x + 2.0f, start_position.y + 2.0f, width - 4.0f, 16.0f },
		THEME_WHITE
	);
	DrawRectangleRec(
		(Rectangle){
			start_position.x + 4.0f,
			start_position.y + 4.0f,
			value * (width - 8.0f),
			12.0f
		},
		THEME_BLACK
	);
}
