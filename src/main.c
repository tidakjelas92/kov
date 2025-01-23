#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

PRIVATE void draw_debug_fps(void) {
	char buffer[10] = {};
	snprintf(buffer, sizeof(buffer), "%d fps", GetFPS());
	DrawText(buffer, 5, 5, 10, THEME_BLACK);
}

GLOBAL b8 paused;
GLOBAL b8 quit_flagged;

PRIVATE void kov_update(void) {
	if (IsKeyPressed(KEY_ESCAPE)) {
		paused = !paused;
		PlaySound(resources_switch_2_sound);
	}

	if (!paused) {
		f32 delta = GetFrameTime();
		scene_update(delta);
	}
}

PRIVATE void kov_render(void) {
	BeginDrawing();
	ClearBackground(THEME_WHITE);

	scene_render();

	if (paused) {
		Vector2 menu_size = { 200.0f, 100.0f };
		i32 result = GuiMessageBox(
			(Rectangle) {
				GetScreenWidth() / 2.0f - menu_size.x / 2.0f,
				GetScreenHeight() / 2.0f - menu_size.y / 2.0f,
				menu_size.x,
				menu_size.y
			},
			"PAUSED",
			"Do you want to quit?",
			"No;#159#Yes"
		);
		if (result == 0 || result == 1) {
			PlaySound(resources_switch_2_sound);
			paused = false;
		} else if (result == 2) {
			quit_flagged = true;
		}
	} else {
		Rectangle esc_src_rect = { 272.0f, 128.0f, 16.0f, 16.0f };
		Vector2 esc_size = { 32, 32 };
		Rectangle esc_dst_rect = { GetScreenWidth() - esc_size.x - 5.0f, 5.0f, esc_size.x, esc_size.y };
		DrawTexturePro(
			resources_prompt_texture,
			esc_src_rect,
			esc_dst_rect,
			Vector2Zero(), 0.0f,
			THEME_BLACK
		);
		const char *pause_text = "Pause";
		Vector2 pause_text_size = MeasureTextEx(GuiGetFont(), pause_text, GuiGetFont().baseSize, 0.0f);
		DrawTextEx(
			GuiGetFont(),
			pause_text,
			(Vector2){
				GetScreenWidth() - pause_text_size.x - 7.0f,
				esc_dst_rect.y + esc_dst_rect.height - 5.0f,
			},
			GuiGetFont().baseSize, 0.0f,
			THEME_BLACK
		);
	}

	draw_debug_fps();
	EndDrawing();
}

int main(void) {
	ChangeDirectory(GetApplicationDirectory());
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "kov");
	InitAudioDevice();
	SetTargetFPS(30);
	SetExitKey(KEY_NULL);

	resources_init();
	GuiLoadStyleDark();

	while (!WindowShouldClose() && !quit_flagged) {
		kov_update();
		kov_render();
	}

	resources_unload();

	CloseAudioDevice();
	CloseWindow();
	return 0;
}
