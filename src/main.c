#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

static void draw_debug_fps(void) {
	char buffer[10] = {};
	snprintf(buffer, sizeof(buffer), "%d fps", GetFPS());
	DrawText(buffer, 5, 5, 12, WHITE);
}

static bool quit_flagged;
static Font pixelplay_font;
static Sound button_press_sound;
static Texture prompt_texture;

static void on_exit_button_pressed(void) {
	PlaySound(button_press_sound);
	quit_flagged = true;
}

static void on_start_button_pressed(void) {
	PlaySound(button_press_sound);
	printf("press start.\n");
}

int main(void) {
	ChangeDirectory(GetApplicationDirectory());
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "kov");
	InitAudioDevice();
	SetTargetFPS(30);
	SetExitKey(KEY_NULL);

	pixelplay_font = LoadFont("assets/fonts/pixelplay.png");
	button_press_sound = LoadSound("assets/sfx/select_002.ogg");
	prompt_texture = LoadTexture("assets/sprites/tilemap_white_packed.png");
	Button start_button = {
		.rect = {
			.x = SCREEN_WIDTH / 2 - 50.0f,
			.y = SCREEN_HEIGHT / 2 - 25.0f,
			.width = 100.0f,
			.height = 50.0f
		},
		.text = "START",
		.font = &pixelplay_font,
		.style = &default_button_style,
		.pressed = &on_start_button_pressed,
	};
	Button exit_button = {
		.rect = {
			.x = SCREEN_WIDTH / 2 - 50.0f,
			.y = SCREEN_HEIGHT / 2 - 25.0F + start_button.rect.height + 10.0f,
			.width = 100.0f,
			.height = 50.0f
		},
		.text = "EXIT",
		.font = &pixelplay_font,
		.style = &default_button_style,
		.pressed = &on_exit_button_pressed,
	};

	Rectangle up_rect = { 480.0f, 192.0f, 16.0f, 16.0f };
	Rectangle right_rect = { 496.0f, 192.0f, 16.0f, 16.0f };
	Rectangle down_rect = { 512.0f, 192.0f, 16.0f, 16.0f };
	Rectangle left_rect = { 528.0f, 192.0f, 16.0f, 16.0f };

	while (!WindowShouldClose() && !quit_flagged) {
		// update game
		// if (IsKeyPressed(KEY_SPACE)) {
			// start_button.state = (start_button.state == BUTTON_STATE_DISABLED) ?
				// BUTTON_STATE_NORMAL : BUTTON_STATE_DISABLED;
		// }

		button_update(&start_button);
		button_update(&exit_button);

		// render
		BeginDrawing();
		ClearBackground((Color){ 240, 227, 184, 255 });

		button_draw(&start_button);
		button_draw(&exit_button);

		DrawTexturePro(
			prompt_texture,
			up_rect,
			(Rectangle){ 50.0f, 50.0f, 100.0f, 100.0f },
			Vector2Zero(), 0.0f,
			(Color){ 110, 100, 90, 255 }
		);
		DrawTexturePro(
			prompt_texture,
			right_rect,
			(Rectangle){ 150.0f, 50.0f, 100.0f, 100.0f },
			Vector2Zero(), 0.0f,
			(Color){ 110, 100, 90, 255 }
		);
		DrawTexturePro(
			prompt_texture,
			down_rect,
			(Rectangle){ 250.0f, 50.0f, 100.0f, 100.0f },
			Vector2Zero(), 0.0f,
			(Color){ 110, 100, 90, 255 }
		);
		DrawTexturePro(
			prompt_texture,
			left_rect,
			(Rectangle){ 350.0f, 50.0f, 100.0f, 100.0f },
			Vector2Zero(), 0.0f,
			(Color){ 110, 100, 90, 255 }
		);

		draw_debug_fps();
		EndDrawing();
	}

	UnloadFont(pixelplay_font);
	CloseAudioDevice();
	CloseWindow();
	return 0;
}
