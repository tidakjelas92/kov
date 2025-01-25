#include "intro.h"
#include "training.h"
#include "ending.h"

typedef enum Scene { SCENE_START, SCENE_INTRO, SCENE_TRAINING, SCENE_GAMEPLAY, SCENE_ENDING } Scene;

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
		TraceLog(LOG_INFO, "enter intro.");
		intro_init();
	} break;
	case SCENE_TRAINING: {
		TraceLog(LOG_INFO, "enter training.");
		training_init();
	} break;
	case SCENE_GAMEPLAY: {
		TraceLog(LOG_INFO, "enter gameplay.");
	} break;
	case SCENE_ENDING: {
		TraceLog(LOG_INFO, "enter ending.");
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
	case SCENE_TRAINING: {
		training_update(delta);
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
	case SCENE_TRAINING: {
		training_render();
	} break;
	case SCENE_ENDING: {
		ending_render();
	} break;
	default: {
	} break;
	}
}
