typedef enum Scene { SCENE_START, SCENE_INTRO, SCENE_GAMEPLAY, SCENE_ENDING } Scene;

GLOBAL Scene scene_current;

PUBLIC void intro_init(void);

PUBLIC void scene_set_scene(Scene scene) {
	if (scene_current == scene) {
		assert(false && "warning: change scene to same scene.");
		return;
	}

	// on exit
	switch (scene_current) {
	case SCENE_INTRO: {

	} break;
	case SCENE_GAMEPLAY: {

	} break;
	case SCENE_ENDING: {

	} break;
	default: {
	} break;
	}

	scene_current = scene;

	// on enter
	switch (scene_current) {
	case SCENE_INTRO: {
		intro_init();
	} break;
	case SCENE_GAMEPLAY: {

	} break;
	case SCENE_ENDING: {

	} break;
	default: {
	} break;
	}
}
