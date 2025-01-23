#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

PRIVATE void draw_debug_fps(void) {
	char buffer[10] = {};
	snprintf(buffer, sizeof(buffer), "%d fps", GetFPS());
	DrawText(buffer, 5, 5, 10, (Color){ 110, 100, 90, 255 });
}

GLOBAL bool quit_flagged;

PRIVATE void kov_update(void) {
	f32 delta = GetFrameTime();
	switch (scene_current) {
	case SCENE_START: {
		scene_set_scene(SCENE_INTRO);
	} break;
	case SCENE_INTRO: {
		intro_update(delta);
	} break;
	case SCENE_GAMEPLAY: {

	} break;
	case SCENE_ENDING: {

	} break;
	default: {
	} break;
	}
}

PRIVATE void kov_render(void) {
	BeginDrawing();
	ClearBackground((Color){ 240, 227, 184, 255 });

	switch (scene_current) {
	case SCENE_INTRO: {
		intro_render();
	} break;
	case SCENE_GAMEPLAY: {

	} break;
	case SCENE_ENDING: {

	} break;
	default: {
	} break;
	}

	// DrawTexturePro(
		// prompt_texture,
		// up_rect,
		// (Rectangle){ 50.0f, 50.0f, 48.0f, 48.0f },
		// Vector2Zero(), 0.0f,
		// (Color){ 110, 100, 90, 255 }
	// );
	// DrawTexturePro(
		// prompt_texture,
		// right_rect,
		// (Rectangle){ 150.0f, 50.0f, 48.0f, 48.0f },
		// Vector2Zero(), 0.0f,
		// (Color){ 110, 100, 90, 255 }
	// );
	// DrawTexturePro(
		// prompt_texture,
		// down_rect,
		// (Rectangle){ 250.0f, 50.0f, 48.0f, 48.0f },
		// Vector2Zero(), 0.0f,
		// (Color){ 110, 100, 90, 255 }
	// );
	// DrawTexturePro(
		// prompt_texture,
		// left_rect,
		// (Rectangle){ 350.0f, 50.0f, 48.0f, 48.0f },
		// Vector2Zero(), 0.0f,
		// (Color){ 110, 100, 90, 255 }
	// );

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

	// Rectangle up_rect = { 480.0f, 192.0f, 16.0f, 16.0f };
	// Rectangle right_rect = { 496.0f, 192.0f, 16.0f, 16.0f };
	// Rectangle down_rect = { 512.0f, 192.0f, 16.0f, 16.0f };
	// Rectangle left_rect = { 528.0f, 192.0f, 16.0f, 16.0f };

	while (!WindowShouldClose() && !quit_flagged) {
		kov_update();
		kov_render();
	}

	resources_unload();

	CloseAudioDevice();
	CloseWindow();
	return 0;
}
