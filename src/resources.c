// global resources

GLOBAL Texture resources_atlas_texture;
GLOBAL Font resources_pixelplay_font;
GLOBAL Font resources_pixel_operator_font;
GLOBAL Sound resources_select_2_sound;
GLOBAL Sound resources_error_5_sound;
GLOBAL Sound resources_switch_2_sound;
GLOBAL Sound resources_scroll_5_sound;
GLOBAL Sound resources_click_1_sound;
GLOBAL Sound resources_click_2_sound;
GLOBAL Sound resources_click_3_sound;
GLOBAL Sound resources_click_4_sound;

PUBLIC void resources_init(void) {
	resources_atlas_texture = LoadTexture("assets/sprites/kov_atlas.png");
	resources_pixelplay_font = LoadFont("assets/fonts/pixelplay.png");
	resources_pixel_operator_font = pixel_operator_font_load();
	resources_select_2_sound = LoadSound("assets/sfx/select_002.ogg");
	resources_error_5_sound = LoadSound("assets/sfx/error_005.ogg");
	resources_switch_2_sound = LoadSound("assets/sfx/switch_002.ogg");
	resources_scroll_5_sound = LoadSound("assets/sfx/scroll_005.ogg");
	resources_click_1_sound = LoadSound("assets/sfx/click_001.ogg");
	resources_click_2_sound = LoadSound("assets/sfx/click_002.ogg");
	resources_click_3_sound = LoadSound("assets/sfx/click_003.ogg");
	resources_click_4_sound = LoadSound("assets/sfx/click_004.ogg");
}

typedef enum ResourcesAtlasTile {
	RESOURCES_ATLAS_TILE_ARROW_UP,
	RESOURCES_ATLAS_TILE_ARROW_RIGHT,
	RESOURCES_ATLAS_TILE_ARROW_DOWN,
	RESOURCES_ATLAS_TILE_ARROW_LEFT,
	RESOURCES_ATLAS_TILE_SPACE,
	RESOURCES_ATLAS_TILE_ESCAPE,
	RESOURCES_ATLAS_TILE_Q,
	RESOURCES_ATLAS_TILE_C,
	RESOURCES_ATLAS_TILE_PLAYER,
	RESOURCES_ATLAS_TILE_GRIMOIRE,
	RESOURCES_ATLAS_TILE_TRAINING_DUMMY_1,
	RESOURCES_ATLAS_TILE_TRAINING_DUMMY_2,
	RESOURCES_ATLAS_TILE_INSTRUCTOR,
	RESOURCES_ATLAS_TILE_GOBLIN,
	RESOURCES_ATLAS_TILE_SKELETON,
	RESOURCES_ATLAS_TILE_MAD_WIZARD,
	RESOURCES_ATLAS_TILE_SWARM,
	RESOURCES_ATLAS_TILE_ARACHNID_QUEEN,
	RESOURCES_ATLAS_TILE_LIVING_ARMOR,
	RESOURCES_ATLAS_TILE_DEMON,
} ResourcesAtlasTile;

// access with enum ResourcesAtlasTile
GLOBAL const Rectangle resources_atlas_rects[] = {
	{ 0, 0, 16, 16 },
	{ 16, 0, 16, 16 },
	{ 32, 0, 16, 16 },
	{ 48, 0, 16, 16 },
	{ 64, 0, 48, 16 },
	{ 112, 0, 16, 16 },
	{ 128, 0, 16, 16 },
	{ 144, 0, 16, 16 },
	{ 0, 16, 16, 16 },
	{ 16, 16, 16, 16 },
	{ 32, 16, 16, 16 },
	{ 48, 16, 16, 16 },
	{ 64, 16, 16, 16 },
	{ 80, 16, 16, 16 },
	{ 96, 16, 16, 16 },
	{ 112, 16, 16, 16 },
	{ 128, 16, 16, 16 },
	{ 144, 16, 16, 16 },
	{ 160, 0, 16, 16 },
	{ 160, 16, 16, 16 },
};
