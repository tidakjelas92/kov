typedef struct ButtonStyle {
	Color normal_color;
	Color normal_font_color;
	Color disabled_color;
	Color disabled_font_color;
	Color hovered_color;
	Color hovered_font_color;
	Color pressed_color;
	Color pressed_font_color;
} ButtonStyle;

typedef enum ButtonState {
	BUTTON_STATE_NORMAL,
	BUTTON_STATE_DISABLED,
	BUTTON_STATE_HOVERED,
	BUTTON_STATE_PRESSED
} ButtonState;

typedef struct Button {
	Rectangle rect;
	const char *text;
	Font *font;
	ButtonStyle *style;
	void (*pressed)(void);
	ButtonState state;
} Button;

static ButtonStyle default_button_style = {
	.normal_color = { 251, 241, 201, 255 },
	.normal_font_color = { 110, 100, 90, 255 },
	.disabled_color = { 224, 207, 169, 255 },
	.disabled_font_color = { 205, 190, 157, 255 },
	.hovered_color = { 255, 252, 240, 255 },
	.hovered_font_color = { 101, 93, 84, 255 },
	.pressed_color = { 255, 128, 25, 255 },
	.pressed_font_color = { 250, 240, 198, 255 },
};

static void button_draw(const Button *button) {
	Color button_color;
	Color font_color;
	switch (button->state) {
	case BUTTON_STATE_NORMAL: {
		button_color = button->style->normal_color;
		font_color = button->style->normal_font_color;
	} break;
	case BUTTON_STATE_DISABLED: {
		button_color = button->style->disabled_color;
		font_color = button->style->disabled_font_color;
	} break;
	case BUTTON_STATE_HOVERED: {
		button_color = button->style->hovered_color;
		font_color = button->style->hovered_font_color;
	} break;
	case BUTTON_STATE_PRESSED: {
		button_color = button->style->pressed_color;
		font_color = button->style->pressed_font_color;
	} break;
	}
	DrawRectangleRec(button->rect, button_color);

	if (button->font != NULL) {
		Vector2 text_size = MeasureTextEx(*button->font, button->text, button->font->baseSize * 2.0f, 2.0f);
		DrawTextEx(
			*button->font,
			button->text,
			(Vector2){
				.x = button->rect.x + button->rect.width / 2.0f - text_size.x / 2.0f,
				.y = button->rect.y + button->rect.height / 2.0f - text_size.y / 2.0f
			},
			button->font->baseSize * 2.0f, 2.0f,
			font_color
		);
	} else {
		Vector2 text_size = MeasureTextEx(GetFontDefault(), button->text, 32.0f, 0.0f);
		DrawText(
			button->text,
			button->rect.x + button->rect.width / 2.0f - text_size.x / 2.0f,
			button->rect.y + button->rect.height / 2.0f - text_size.y / 2.0f,
			32.0f,
			font_color
		);
	}
}

static void button_update(Button *button) {
	if (button->state != BUTTON_STATE_DISABLED) {
		if (CheckCollisionPointRec(GetMousePosition(), button->rect)) {
			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
				button->state = BUTTON_STATE_PRESSED;
			} else {
				button->state = BUTTON_STATE_HOVERED;
			}

			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && button->pressed != NULL) {
				button->pressed();
			}
		} else {
			button->state = BUTTON_STATE_NORMAL;
		}
	}
}
