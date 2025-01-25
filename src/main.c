#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

PRIVATE void draw_debug_fps(void) {
	char buffer[10] = { 0 };
	snprintf(buffer, sizeof(buffer), "%d fps", GetFPS());
	DrawText(buffer, 5, 5, 10, THEME_BLACK);
}

PRIVATE void kov_update(void) {
	f32 delta = GetFrameTime();
	scene_update(delta);
}

PRIVATE void kov_render(void) {
	BeginDrawing();
	ClearBackground(THEME_WHITE);

	scene_render();
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

	while (!WindowShouldClose() && !quit_flagged) {
		kov_update();
		kov_render();
	}

	resources_unload();

	CloseAudioDevice();
	CloseWindow();
	return 0;
}
