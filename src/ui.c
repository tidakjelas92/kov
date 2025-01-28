// draw functions for ui components.

PUBLIC void ui_draw_pause_menu(void) {
	Vector2 position = { 100.0f, 100.0f };
	DrawRectangle(
		position.x,
		position.y,
		GetScreenWidth() - position.x * 2.0f,
		GetScreenHeight() - position.y * 2.0f,
		THEME_BLACK
	);

	const char *pause_text = "PAUSED";
	Vector2 pause_text_size = MeasureTextEx(resources_pixelplay_font, pause_text, resources_pixelplay_font.baseSize * 4.0f, 4.0f);
	DrawTextEx(
		resources_pixelplay_font,
		pause_text,
		(Vector2){
			GetScreenWidth() / 2.0f - pause_text_size.x / 2.0f,
			position.y + 100.0f
		},
		resources_pixelplay_font.baseSize * 4.0f, 4.0f,
		THEME_WHITE
	);

	Vector2 resume_position = {
		GetScreenWidth() / 2.0f - 50.0f,
		GetScreenHeight() / 2.0f
	};
	Vector2 esc_size = { 32.0f, 32.0f };
	Rectangle esc_dst_rect = {
		resume_position.x - esc_size.x / 2.0f,
		resume_position.y,
		esc_size.x, esc_size.y
	};
	DrawTexturePro(
		resources_prompt_texture,
		prompt_tiles[PROMPT_TILE_ESCAPE],
		esc_dst_rect,
		Vector2Zero(), 0.0f,
		THEME_WHITE
	);
	const char *resume_text = "RESUME";
	Vector2 resume_text_size = MeasureTextEx(resources_pixel_operator_font, resume_text, resources_pixel_operator_font.baseSize, 0.0f);
	DrawTextEx(
		resources_pixel_operator_font,
		resume_text,
		(Vector2){
			resume_position.x - resume_text_size.x / 2.0f,
			resume_position.y + esc_size.y
		},
		resources_pixel_operator_font.baseSize, 0.0f,
		THEME_WHITE
	);

	Vector2 quit_position = {
		GetScreenWidth() / 2.0f + 50.0f,
		GetScreenHeight() / 2.0f
	};
	Vector2 q_size = { 32.0f, 32.0f };
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
		THEME_WHITE
	);
	const char *quit_text = "QUIT";
	Vector2 quit_text_size = MeasureTextEx(resources_pixel_operator_font, quit_text, resources_pixel_operator_font.baseSize, 0.0f);
	DrawTextEx(
		resources_pixel_operator_font,
		quit_text,
		(Vector2){
			quit_position.x - quit_text_size.x / 2.0f,
			quit_position.y + q_size.y
		},
		resources_pixel_operator_font.baseSize, 0.0f,
		THEME_WHITE
	);
}

PUBLIC void ui_draw_space_confirm(Color color) {
	Vector2 space_dst_size = { 96.0f, 32.0f };
	f32 text_spacing = 3.0f;
	Rectangle space_dst_rect = {
		GetScreenWidth() / 2.0f - space_dst_size.x / 2.0f,
		GetScreenHeight() - space_dst_size.y - 30.0f,
		space_dst_size.x,
		space_dst_size.y
	};

	if (IsKeyDown(KEY_SPACE)) {
		space_dst_rect.y += 4.0f;
	}

	DrawTexturePro(
		resources_prompt_texture,
		prompt_tiles[PROMPT_TILE_SPACE],
		space_dst_rect,
		Vector2Zero(), 0.0f,
		color
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
		color
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
