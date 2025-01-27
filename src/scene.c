#include "intro.h"
#include "gameplay.h"
#include "game_over.h"
#include "ending.h"

typedef enum Scene {
	SCENE_START, SCENE_INTRO, SCENE_GAMEPLAY, SCENE_GAME_OVER, SCENE_ENDING
} Scene;

GLOBAL Scene scene_current;

PUBLIC void scene_set_scene(Scene scene) {
	if (scene_current == scene) {
		TraceLog(LOG_WARNING, "changing scene to the same scene.");
		return;
	}

	// on exit
	scene_current = scene;

	// on enter
	switch (scene_current) {
	case SCENE_INTRO: {
		TraceLog(LOG_DEBUG, "enter intro.");
		intro_init();
	} break;
	case SCENE_GAMEPLAY: {
		TraceLog(LOG_DEBUG, "enter gameplay.");
		gameplay_init();
	} break;
	case SCENE_GAME_OVER: {
		TraceLog(LOG_DEBUG, "enter game over.");
		game_over_init();
	} break;
	case SCENE_ENDING: {
		TraceLog(LOG_DEBUG, "enter ending.");
		ending_init();
	} break;
	default: {
	} break;
	}
}

PUBLIC void scene_update(f32 delta) {
	switch (scene_current) {
	case SCENE_START: {
		scene_set_scene(SCENE_INTRO);
	} break;
	case SCENE_INTRO: {
		intro_update(delta);
	} break;
	case SCENE_GAMEPLAY: {
		gameplay_update(delta);
	} break;
	case SCENE_GAME_OVER: {
		game_over_update(delta);
	} break;
	case SCENE_ENDING: {
		ending_update(delta);
	} break;
	default: {
	} break;
	}
}

PUBLIC void scene_render(void) {
	switch (scene_current) {
	case SCENE_INTRO: {
		intro_render();
	} break;
	case SCENE_GAMEPLAY: {
		gameplay_render();
	} break;
	case SCENE_GAME_OVER: {
		game_over_render();
	} break;
	case SCENE_ENDING: {
		ending_render();
	} break;
	default: {
	} break;
	}
}
