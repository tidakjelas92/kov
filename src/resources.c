// global resources

GLOBAL Texture resources_prompt_texture;
GLOBAL Texture resources_onebit_texture;
GLOBAL Font resources_pixelplay_font;
GLOBAL Font resources_pixel_operator_font;
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
	resources_onebit_texture = LoadTexture("assets/sprites/monochrome-transparent_packed.png");
	resources_pixelplay_font = LoadFont("assets/fonts/pixelplay.png");
	resources_pixel_operator_font = LoadFontEx("assets/fonts/PixelOperator.ttf", 16, NULL, 0);
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

typedef enum PromptTile {
	PROMPT_TILE_ARROW_UP,
	PROMPT_TILE_ARROW_RIGHT,
	PROMPT_TILE_ARROW_DOWN,
	PROMPT_TILE_ARROW_LEFT,
	PROMPT_TILE_SPACE,
	PROMPT_TILE_ESCAPE,
	PROMPT_TILE_Q
} PromptTile;

// access with PromptTile as index
GLOBAL const Rectangle prompt_tiles[] = {
	{ 480, 192, 16, 16 },
	{ 496, 192, 16, 16 },
	{ 512, 192, 16, 16 },
	{ 528, 192, 16, 16 },
	{ 496, 224, 48, 16 },
	{ 272, 128, 16, 16 },
	{ 272, 160, 16, 16 }
};

typedef enum OnebitTile {
	ONEBIT_TILE_PLAYER,
} OnebitTile;

GLOBAL const Rectangle onebit_tiles[] = {
	{ 432, 0, 16, 16 },
};
