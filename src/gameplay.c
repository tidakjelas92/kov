// TODO: allocate context with gameplay arena.
GLOBAL GameContext game_context;

GLOBAL const AttackInfo gameplay_attack_infos[] = {
	{ "??", { 0, 0, 0, 0, 0, 0, 0, 0 }, 0, ATTACK_TYPE_SINGLE },
	{ "Slash", { 1, 2, 2, 0, 0, 0, 0, 0 }, 5, ATTACK_TYPE_SINGLE },
	{ "Cross Slash", { 1, 2, 2, 1, 4, 4, 0, 0 }, 12, ATTACK_TYPE_SINGLE },
	{ "Twirl", { 2, 3, 4, 2, 0, 0, 0, 0 }, 4, ATTACK_TYPE_AOE },
	{ "Spear Thrust", { 2, 2, 2, 2, 2, 2, 2, 2 }, 15, ATTACK_TYPE_SPLASH }
};
#define GAMEPLAY_ATTACK_INFOS_LEN sizeof(gameplay_attack_infos) / sizeof(AttackInfo)

GLOBAL u8 gameplay_known_attacks[GAMEPLAY_ATTACK_INFOS_LEN];

GLOBAL const StageInfo gameplay_stage_infos[] = {
	{ .type = STAGE_TYPE_GRIMOIRE, .data = { .grimoire_data = { 1 } } },
	{ .type = STAGE_TYPE_BATTLE, .data = { .battle_data = { { 0, 0, 0, 0, 0 }, 1 } } },
	{ .type = STAGE_TYPE_BATTLE, .data = { .battle_data = { { 0, 1, 0, 0, 0 }, 3 } } },
	{ .type = STAGE_TYPE_BATTLE, .data = { .battle_data = { { 2, 0, 0, 0, 0 }, 1 } } },
	{ .type = STAGE_TYPE_GRIMOIRE, .data = { .grimoire_data = { 2 } } },
	{ .type = STAGE_TYPE_BATTLE, .data = { .battle_data = { { 3, 3, 0, 0, 0 }, 2 } } },
	{ .type = STAGE_TYPE_BATTLE, .data = { .battle_data = { { 3, 3, 3, 3, 3 }, 5 } } },
	{ .type = STAGE_TYPE_BATTLE, .data = { .battle_data = { { 4, 4, 5, 0, 0 }, 3 } } },
	{ .type = STAGE_TYPE_GRIMOIRE, .data = { .grimoire_data = { 3 } } },
	{ .type = STAGE_TYPE_BATTLE, .data = { .battle_data = { { 6, 6, 6, 6, 6 }, 5 } } },
	{ .type = STAGE_TYPE_BATTLE, .data = { .battle_data = { { 7, 0, 0, 0, 0 }, 1 } } },
	{ .type = STAGE_TYPE_BATTLE, .data = { .battle_data = { { 8, 8, 0, 0, 0 }, 2 } } },
	{ .type = STAGE_TYPE_GRIMOIRE, .data = { .grimoire_data = { 4 } } },
	{ .type = STAGE_TYPE_BATTLE, .data = { .battle_data = { { 4, 4, 4, 4, 9 }, 5 } } },
};
#define GAMEPLAY_STAGE_INFOS_LEN sizeof(gameplay_stage_infos) / sizeof(StageInfo)

GLOBAL const f32 gameplay_input_times[] = { 4.0f, 3.5f, 1.5f, 2.5f };
#define GAMEPLAY_INPUT_TIMES_LEN sizeof(gameplay_input_times) / sizeof(f32)

PUBLIC void gameplay_init(void) {
	TraceLog(LOG_INFO, "sizeof GamePhase: %zu", sizeof(GamePhase));
	TraceLog(LOG_INFO, "sizeof attack_infos: %zu", sizeof(gameplay_attack_infos));
	TraceLog(LOG_INFO, "sizeof StageInfo: %zu", sizeof(StageInfo));
	TraceLog(LOG_INFO, "sizeof stage_infos: %zu", sizeof(gameplay_stage_infos));
	TraceLog(LOG_INFO, "sizeof GameContext: %zu", sizeof(GameContext));

	game_context = (GameContext) {
		.input_times = gameplay_input_times,
		.input_times_len = GAMEPLAY_INPUT_TIMES_LEN,
		.player_health = MAX_PLAYER_HEALTH,
		.stage_infos = gameplay_stage_infos,
		.stage_infos_len = GAMEPLAY_STAGE_INFOS_LEN,
		.attack_infos = gameplay_attack_infos,
		.attack_infos_len = GAMEPLAY_ATTACK_INFOS_LEN,
		.known_attacks = gameplay_known_attacks,
	};

	app_paused = false;

	game_transition_stage_type(&game_context);
}

PUBLIC void gameplay_update(f32 delta) {
	if (IsKeyPressed(KEY_ESCAPE)) {
		app_paused = !app_paused;
		PlaySound(resources_switch_2_sound);
	}

	if (app_paused) {
		if (IsKeyPressed(KEY_Q)) {
			scene_set_scene(SCENE_INTRO);
		}
	} else {
		game_update(&game_context, delta);
		switch (game_context.phase) {
		case GAME_PHASE_LOSE: {
			scene_set_scene(SCENE_GAME_OVER);
		} break;
		case GAME_PHASE_WIN: {
			scene_set_scene(SCENE_ENDING);
		} break;
		default: {
		} break;
		}
	}
}

PRIVATE const char *get_attack_type_string(AttackType type) {
	switch (type) {
	case ATTACK_TYPE_SINGLE: {
		return "Single";
	} break;
	case ATTACK_TYPE_AOE: {
		return "AoE";
	} break;
	case ATTACK_TYPE_SPLASH: {
		return "Splash";
	} break;
	}
}

PRIVATE void gameplay_render_known_attacks(GameContext *context) {
	Vector2 position = { 20, 20 };
	f32 row_height = 20.0f;
	for (u32 i = 0; i < context->known_attacks_len; i++) {
		const AttackInfo *attack_info = &context->attack_infos[context->known_attacks[i]];
		char text[24] = { 0 };
		snprintf(text, sizeof(text), "%s (%s)", attack_info->name, get_attack_type_string(attack_info->type));
	Vector2 text_size = MeasureTextEx(resources_pixel_operator_font, text, resources_pixel_operator_font.baseSize, 0.0f);
		DrawTextEx(
			resources_pixel_operator_font,
			text,
			position,
			resources_pixel_operator_font.baseSize, 0.0f,
			THEME_BLACK
		);

		Vector2 arrow_dst_size = { 16.0f, 16.0f };
		const Sequence *sequence = &attack_info->sequence;
		u8 sequence_len = sequence_get_len(sequence);
		for (u32 j = 0; j < sequence_len; j++) {
			Rectangle arrow_dst_rect = {
				position.x + text_size.x + 5.0f + arrow_dst_size.x * j,
				position.y,
				arrow_dst_size.x,
				arrow_dst_size.y
			};
			DrawTexturePro(
				resources_prompt_texture,
				// basically works because the value is literally the same.
				prompt_tiles[sequence->buffer[j] - 1],
				arrow_dst_rect,
				Vector2Zero(), 0.0f,
				THEME_BLACK
			);
		}
		position.y += row_height;
	}
}

PRIVATE void gameplay_render_enemies(GameContext *context) {
	const StageInfo *stage_info = &context->stage_infos[context->stage];
	Vector2 position = { GetScreenWidth() / 2.0f + 300.0f, 300.0f };
	Vector2 size = { 32, 32 };
	for (i32 i = stage_info->data.battle_data.enemies_len - 1; i >= 0 ; i--) {
		position.x -= size.x;

		if (context->enemy_healths[i] > 0) {
			const EnemyInfo *enemy_info = &enemy_infos[stage_info->data.battle_data.enemy_ids[i]];
			DrawTexturePro(
				resources_onebit_texture,
				onebit_tiles[enemy_info->tile],
				(Rectangle){ position.x, position.y, size.x, size.y },
				Vector2Zero(), 0.0f,
				THEME_BLACK
			);
		}
	}

	Vector2 bars_position = { GetScreenWidth() / 2.0f + 220.0f, 120.0f };
	for (u32 i = 0; i < stage_info->data.battle_data.enemies_len; i++) {
		ui_draw_health_bar(
			bars_position,
			160,
			(f32)game_context.enemy_healths[i] / (f32)enemy_infos[stage_info->data.battle_data.enemy_ids[i]].health
		);
		bars_position.y += 8.0f;

		const EnemyInfo *enemy_info = &enemy_infos[stage_info->data.battle_data.enemy_ids[i]];
		Vector2 text_size = MeasureTextEx(resources_pixel_operator_font, enemy_info->name, resources_pixel_operator_font.baseSize, 0.0f);
		DrawTextEx(
			resources_pixel_operator_font,
			enemy_info->name,
			(Vector2){
				bars_position.x + 80.0f - text_size.x,
				bars_position.y
			},
			resources_pixel_operator_font.baseSize, 0.0f,
			THEME_BLACK
		);
		bars_position.y += text_size.y + 12.0f;
	}
}

PRIVATE void gameplay_render_active_sequence(GameContext *context) {
	Vector2 arrow_dst_size = { 48.0f, 48.0f };
	Vector2 start_position = {
		GetScreenWidth() / 2.0f - arrow_dst_size.x * context->active_position / 2.0f,
		GetScreenHeight() - 120.0f - arrow_dst_size.y
	};

	for (u32 i = 0; i < context->active_position; i++) {
		Rectangle arrow_dst_rect = {
			start_position.x + arrow_dst_size.x * i,
			start_position.y,
			arrow_dst_size.x,
			arrow_dst_size.y
		};
		DrawTexturePro(
			resources_prompt_texture,
			// basically works because the value is literally the same.
			prompt_tiles[context->active_sequence.buffer[i] - 1],
			arrow_dst_rect,
			Vector2Zero(), 0.0f,
			THEME_BLACK
		);
	}
}

PRIVATE void gameplay_render_attack_queue(GameContext *context) {
	if (context->attack_count > 0) {
		const AttackInfo *attack_info = &context->attack_infos[context->attack_queue[context->attack_count - 1]];
		char text[24] = { 0 };
		snprintf(text, sizeof(text), "(%d) %s", context->attack_count, attack_info->name);
		Vector2 text_size = MeasureTextEx(resources_pixel_operator_font, text, resources_pixel_operator_font.baseSize * 2.0f, 2.0f);
		DrawTextEx(
			resources_pixel_operator_font,
			text,
			(Vector2){
				GetScreenWidth() / 2.0f - text_size.x / 2.0f,
				GetScreenHeight() / 2.0f + 50.0f,
			},
			resources_pixel_operator_font.baseSize * 2.0f, 0.0f,
			THEME_BLACK
		);
	}
}

PRIVATE void gameplay_render_current_attack(const GameContext *context, const AttackInfo *attack_info) {
	if (context->attack_count > 0 && context->attack_position < context->attack_count) {
		Vector2 panel_size = { 200, 50 };
		Rectangle panel_rect = {
			GetScreenWidth() / 2.0f - panel_size.x / 2.0f,
			150,
			panel_size.x,
			panel_size.y
		};
		DrawRectangle(panel_rect.x - 5, panel_rect.y - 5, panel_size.x + 10, panel_size.y + 10, THEME_BLACK);

		Vector2 text_size = MeasureTextEx(resources_pixel_operator_font, attack_info->name, resources_pixel_operator_font.baseSize * 3.0f, 2.0f);
		DrawTextEx(
			resources_pixel_operator_font,
			attack_info->name,
			(Vector2){
				GetScreenWidth() / 2.0f - text_size.x / 2.0f,
				150.0f,
			},
			resources_pixel_operator_font.baseSize * 3.0f, 0.0f,
			THEME_WHITE
		);
	}
}

PRIVATE void gameplay_render_enemy_attack(const GameContext *context, const EnemyAttackInfo *enemy_attack_info) {
	if (context->enemy_attack_position < MAX_ENEMIES_PER_STAGE) {
		Vector2 panel_size = { 200, 50 };
		Rectangle panel_rect = {
			GetScreenWidth() / 2.0f - panel_size.x / 2.0f,
			150,
			panel_size.x,
			panel_size.y
		};
		DrawRectangle(panel_rect.x - 5, panel_rect.y - 5, panel_size.x + 10, panel_size.y + 10, THEME_RED);

		Vector2 text_size = MeasureTextEx(resources_pixel_operator_font, enemy_attack_info->name, resources_pixel_operator_font.baseSize * 3.0f, 2.0f);
		DrawTextEx(
			resources_pixel_operator_font,
			enemy_attack_info->name,
			(Vector2){
				GetScreenWidth() / 2.0f - text_size.x / 2.0f,
				panel_rect.y + panel_size.y / 2.0f - text_size.y / 2.0f,
			},
			resources_pixel_operator_font.baseSize * 3.0f, 0.0f,
			THEME_WHITE
		);
	}
}

PRIVATE void gameplay_render_input_time(GameContext *context) {
	f32 input_time = context->input_times[context->input_time_position];
	f32 time_bar_width = (input_time - context->elapsed) / input_time * GetScreenWidth();
	DrawRectangle(
		GetScreenWidth() / 2.0f - time_bar_width / 2.0f,
		GetScreenHeight() - 10,
		time_bar_width,
		10,
		THEME_BLACK
	);
}

PUBLIC void gameplay_render(void) {
	char stage_text[6];
	snprintf(stage_text, sizeof(stage_text),"%u/%u", game_context.stage + 1, game_context.stage_infos_len);
	Vector2 stage_text_size = MeasureTextEx(resources_pixelplay_font, stage_text, resources_pixelplay_font.baseSize * 3.0f, 4.0f);
	DrawTextEx(
		resources_pixelplay_font,
		stage_text,
		(Vector2){ GetScreenWidth() / 2.0f - stage_text_size.x / 2.0f, 10.0f },
		resources_pixelplay_font.baseSize * 3.0f, 4.0f,
		THEME_BLACK
	);

	DrawTexturePro(
		resources_onebit_texture,
		onebit_tiles[ONEBIT_TILE_PLAYER],
		(Rectangle){ GetScreenWidth() / 2.0f - 300.0f, 300.0f, 32.0f, 32.0f },
		Vector2Zero(), 0.0f,
		THEME_BLACK
	);
	ui_draw_health_bar(
		(Vector2){ GetScreenWidth() / 2.0f - 220.0f, 120 },
		160,
		(f32)game_context.player_health / (f32)MAX_PLAYER_HEALTH
	);

	gameplay_render_known_attacks(&game_context);
	gameplay_render_enemies(&game_context);

	switch (game_context.phase) {
	case GAME_PHASE_PREPARE: {
		const char *text = "Ready?";
		f32 text_font_size = resources_pixelplay_font.baseSize * 2.0f;
		f32 text_spacing = 3.0f;
		Vector2 text_size = MeasureTextEx(resources_pixelplay_font, text, text_font_size, text_spacing);

		DrawTextEx(
			resources_pixelplay_font,
			text,
			(Vector2){
				GetScreenWidth() / 2.0f - text_size.x / 2.0f,
				GetScreenHeight() / 2.0f - text_size.y / 2.0f
			},
			text_font_size, text_spacing,
			THEME_BLACK
		);
	} break;
	case GAME_PHASE_INPUT: {
		gameplay_render_active_sequence(&game_context);
		gameplay_render_attack_queue(&game_context);
		gameplay_render_input_time(&game_context);
		if (!app_paused) {
			ui_draw_space_confirm(THEME_BLACK);
		}
	} break;
	case GAME_PHASE_ATTACK_PLAYER: {
		gameplay_render_current_attack(&game_context, &game_context.attack_infos[game_context.attack_queue[game_context.attack_position]]);
	} break;
	case GAME_PHASE_ATTACK_ENEMY: {
		if (game_context.enemy_healths[game_context.enemy_attack_position] > 0) {
			const StageInfo *stage_info = &game_context.stage_infos[game_context.stage];
			const EnemyInfo *enemy_info = &enemy_infos[stage_info->data.battle_data.enemy_ids[game_context.enemy_attack_position]];
			gameplay_render_enemy_attack(&game_context, &enemy_attack_infos[enemy_info->attack_id]);
		}
	} break;
	case GAME_PHASE_GRIMOIRE_CONTINUE: {
		if (!app_paused) {
			ui_draw_space_confirm(THEME_BLACK);
		}
	} break;
	default: {
	} break;
	}

	if (game_context.phase == GAME_PHASE_GRIMOIRE_WAIT || game_context.phase == GAME_PHASE_GRIMOIRE_CONTINUE) {
		Vector2 panel_size = { 450, 300 };
		Rectangle panel_rect = {
			GetScreenWidth() / 2.0f - panel_size.x / 2.0f,
			GetScreenHeight() / 2.0f - panel_size.y / 2.0f,
			panel_size.x,
			panel_size.y
		};
		DrawRectangle(panel_rect.x - 5, panel_rect.y - 5, panel_size.x + 10, panel_size.y + 10, THEME_WHITE);
		DrawRectangleRec(panel_rect, THEME_BLACK);

		Vector2 grim_size = { 80, 80 };
		Rectangle grim_dst_rect = {
			panel_rect.x + panel_rect.width / 2.0f - grim_size.x / 2.0f,
			panel_rect.y + 50.0f,
			grim_size.x,
			grim_size.y
		};
		DrawTexturePro(
			resources_onebit_texture,
			onebit_tiles[ONEBIT_TILE_GRIMOIRE],
			grim_dst_rect,
			Vector2Zero(), 0.0f,
			THEME_WHITE
		);
		const char *text = "Grimoire Found!";
		Vector2 text_size = MeasureTextEx(resources_pixel_operator_font, text, resources_pixel_operator_font.baseSize * 3.0f, 2.0f);
		Vector2 text_position = {
			GetScreenWidth() / 2.0f - text_size.x / 2.0f,
			grim_dst_rect.y + grim_dst_rect.height,
		};
		DrawTextEx(
			resources_pixel_operator_font,
			text,
			text_position,
			resources_pixel_operator_font.baseSize * 3.0f, 2.0f,
			THEME_WHITE
		);

		const StageInfo *stage_info = &game_context.stage_infos[game_context.stage];
		const AttackInfo *attack_info = &game_context.attack_infos[stage_info->data.grimoire_data.attack_id];
		char name_text[24] = { 0 };
		snprintf(name_text, sizeof(name_text), "%s (%s)", attack_info->name, get_attack_type_string(attack_info->type));
		Vector2 name_text_size = MeasureTextEx(resources_pixel_operator_font, name_text, resources_pixel_operator_font.baseSize * 2.0f, 2.0f);
		Vector2 name_text_position = {
			GetScreenWidth() / 2.0f - name_text_size.x / 2.0f,
			text_position.y + text_size.y,
		};
		DrawTextEx(
			resources_pixel_operator_font,
			name_text,
			name_text_position,
			resources_pixel_operator_font.baseSize * 2.0f, 2.0f,
			THEME_WHITE
		);

		const Sequence *sequence = &attack_info->sequence;
		u8 sequence_len = sequence_get_len(sequence);
		Vector2 arrow_dst_size = { 48.0f, 48.0f };
		Vector2 sequence_start_position = {
			GetScreenWidth() / 2.0f - arrow_dst_size.x * sequence_len / 2.0f,
			name_text_position.y + name_text_size.y
		};

		for (u32 i = 0; i < sequence_len; i++) {
			Rectangle arrow_dst_rect = {
				sequence_start_position.x + arrow_dst_size.x * i,
				sequence_start_position.y,
				arrow_dst_size.x,
				arrow_dst_size.y
			};
			DrawTexturePro(
				resources_prompt_texture,
				// basically works because the value is literally the same.
				prompt_tiles[sequence->buffer[i] - 1],
				arrow_dst_rect,
				Vector2Zero(), 0.0f,
				THEME_WHITE
			);
		}
	}

	if (app_paused) {
		ui_draw_pause_menu();
	} else {
		Vector2 esc_size = { 32, 32 };
		Rectangle esc_dst_rect = { GetScreenWidth() - esc_size.x - 5.0f, 5.0f, esc_size.x, esc_size.y };
		DrawTexturePro(
			resources_prompt_texture,
			prompt_tiles[PROMPT_TILE_ESCAPE],
			esc_dst_rect,
			Vector2Zero(), 0.0f,
			THEME_BLACK
		);
		const char *pause_text = "Pause";
		Vector2 pause_text_size = MeasureTextEx(resources_pixel_operator_font, pause_text, resources_pixel_operator_font.baseSize, 0.0f);
		DrawTextEx(
			resources_pixel_operator_font,
			pause_text,
			(Vector2){
				GetScreenWidth() - pause_text_size.x - 7.0f,
				esc_dst_rect.y + esc_dst_rect.height - 5.0f,
			},
			resources_pixel_operator_font.baseSize, 0.0f,
			THEME_BLACK
		);
	}
}
