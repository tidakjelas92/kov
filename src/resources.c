// global resources

GLOBAL Texture resources_prompt_texture;
GLOBAL Font resources_pixelplay_font;

PUBLIC void resources_init(void) {
	resources_prompt_texture = LoadTexture("assets/sprites/tilemap_white_packed.png");
	resources_pixelplay_font = LoadFont("assets/fonts/pixelplay.png");
}

PUBLIC void resources_unload(void) {
	UnloadFont(resources_pixelplay_font);
}
