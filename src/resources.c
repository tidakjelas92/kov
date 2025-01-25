// global resources

GLOBAL Texture resources_prompt_texture;
GLOBAL Texture resources_rpg_texture;
GLOBAL Font resources_pixelplay_font;
GLOBAL Sound resources_select_2_sound;
GLOBAL Sound resources_error_1_sound;
GLOBAL Sound resources_error_5_sound;
GLOBAL Sound resources_switch_2_sound;
GLOBAL Sound resources_scroll_5_sound;
GLOBAL Sound resources_click_1_sound;
GLOBAL Sound resources_click_2_sound;
GLOBAL Sound resources_click_3_sound;
GLOBAL Sound resources_click_4_sound;

PUBLIC void resources_init(void) {
	resources_prompt_texture = LoadTexture("assets/sprites/tilemap_white_packed.png");
  resources_rpg_texture = LoadTexture("assets/sprites/tilemap_packed.png");
	resources_pixelplay_font = LoadFont("assets/fonts/pixelplay.png");
	resources_select_2_sound = LoadSound("assets/sfx/select_002.ogg");
	resources_error_1_sound = LoadSound("assets/sfx/error_001.ogg");
	resources_error_5_sound = LoadSound("assets/sfx/error_005.ogg");
	resources_switch_2_sound = LoadSound("assets/sfx/switch_002.ogg");
	resources_scroll_5_sound = LoadSound("assets/sfx/scroll_005.ogg");
	resources_click_1_sound = LoadSound("assets/sfx/click_001.ogg");
	resources_click_2_sound = LoadSound("assets/sfx/click_002.ogg");
	resources_click_3_sound = LoadSound("assets/sfx/click_003.ogg");
	resources_click_4_sound = LoadSound("assets/sfx/click_004.ogg");
}

PUBLIC void resources_unload(void) {
	UnloadFont(resources_pixelplay_font);
}
