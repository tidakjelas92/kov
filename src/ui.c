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
