// TODO: allocate context with gameplay arena.
GLOBAL GameContext game_context;

typedef struct EnemyAttackInfo {
	const char *name;
	u16 damage;
} EnemyAttackInfo;

GLOBAL const EnemyAttackInfo enemy_attack_infos[] = {
	{ "Stare", 0 },
	{ "Hit", 2 },
	{ "Scratch", 1 },
	{ "Wild Swing", 3 },
	{ "Fireball", 5 },
	{ "Venom", 10 },
	{ "Demonic Breath", 7 }
};
#define ENEMY_ATTACK_INFOS_COUNT sizeof(enemy_attack_infos) / sizeof(EnemyAttackInfo)

typedef struct EnemyInfo {
	const char *name;
	u16 health;
	u8 attack_id;
	u8 tile;
} EnemyInfo;

GLOBAL const EnemyInfo enemy_infos[] = {
	{ "Training Dummy 1", 10, 0, ONEBIT_TILE_TRAINING_DUMMY_1 },
	{ "Training Dummy 2", 15, 0, ONEBIT_TILE_TRAINING_DUMMY_2 },
	{ "Instructor", 30, 1, ONEBIT_TILE_INSTRUCTOR },
	{ "Goblin", 6, 1, ONEBIT_TILE_GOBLIN },
	{ "Skeleton", 3, 1, ONEBIT_TILE_SKELETON },
	{ "Mad Wizard", 10, 4, ONEBIT_TILE_MAD_WIZARD },
	{ "Swarm", 1, 2, ONEBIT_TILE_SWARM },
	{ "Arachnid Queen", 30, 5, ONEBIT_TILE_ARACHNID_QUEEN },
	{ "Living Armor", 20, 3, ONEBIT_TILE_LIVING_ARMOR },
	{ "Demon", 50, 6, ONEBIT_TILE_DEMON }
};
#define ENEMY_INFOS_LEN sizeof(enemy_infos) / sizeof(EnemyInfo)

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

PUBLIC void game_set_phase(GamePhase phase) {
	if (game_context.phase == phase) {
		TraceLog(LOG_WARNING, "changing phase to the same phase.");
		return;
	}

	// on exit
	switch (game_context.phase) {
	case GAME_PHASE_INPUT: {
		// scroll through input_time
		game_context.input_time_position += 1;
		if (game_context.input_time_position >= game_context.input_times_len) {
			game_context.input_time_position = 0;
		}
	} break;
	default: {
	} break;
	}
	game_context.phase = phase;

	// on enter
	switch (game_context.phase) {
	case GAME_PHASE_PREPARE: {
		game_context.elapsed = 0.0f;
	} break;
	case GAME_PHASE_INPUT: {
		memset(&game_context.attack_queue, 0, MAX_ATTACK_PER_TURN);
		memset(&game_context.active_sequence, 0, sizeof(Sequence));
		game_context.active_position = 0;
		game_context.elapsed = 0.0f;
		game_context.attack_count = 0;
	} break;
	case GAME_PHASE_ATTACK_PLAYER: {
		game_context.elapsed = 0.0f;
		game_context.attack_position = 0;
	} break;
	case GAME_PHASE_ATTACK_ENEMY: {
		game_context.elapsed = 0.0f;
		game_context.enemy_attack_position = 0;
	} break;
	case GAME_PHASE_GRIMOIRE_WAIT: {
		game_context.elapsed = 0.0f;
	} break;
	case GAME_PHASE_GRIMOIRE_CONTINUE: {
		u8 attack_id = game_context.stage_infos[game_context.stage].data.grimoire_data.attack_id;
		game_context.known_attacks[game_context.known_attacks_len] = attack_id;
		game_context.known_attacks_len += 1;
	} break;
	default: {
	} break;
	}
}

PRIVATE void game_transition_stage_type(GameContext *context) {
	const StageInfo *stage_info = &context->stage_infos[context->stage];
	switch (stage_info->type) {
	case STAGE_TYPE_BATTLE: {
		game_set_phase(GAME_PHASE_PREPARE);
		// prepare the enemies' health values
		const StageInfo *stage_info = &context->stage_infos[context->stage];
		for (u32 i = 0; i < stage_info->data.battle_data.enemies_len; i++) {
			const EnemyInfo *enemy_info = &enemy_infos[stage_info->data.battle_data.enemy_ids[i]];
			context->enemy_healths[i] = enemy_info->health;
		}
	} break;
	case STAGE_TYPE_GRIMOIRE: {
		game_set_phase(GAME_PHASE_GRIMOIRE_WAIT);
	} break;
	}
}

PUBLIC void gameplay_init(void) {
	TraceLog(LOG_INFO, "sizeof GamePhase: %zu", sizeof(GamePhase));
	TraceLog(LOG_INFO, "sizeof attack_infos: %zu", sizeof(attack_infos));
	TraceLog(LOG_INFO, "sizeof StageInfo: %zu", sizeof(StageInfo));
	TraceLog(LOG_INFO, "sizeof stage_infos: %zu", sizeof(gameplay_stage_infos));
	TraceLog(LOG_INFO, "sizeof GameContext: %zu", sizeof(GameContext));

	game_context = (GameContext) {
		.input_times = gameplay_input_times,
		.input_times_len = GAMEPLAY_INPUT_TIMES_LEN,
		.player_health = MAX_PLAYER_HEALTH,
		.stage_infos = gameplay_stage_infos,
		.stage_infos_len = GAMEPLAY_STAGE_INFOS_LEN,
	};

	app_paused = false;

	game_transition_stage_type(&game_context);
}

PRIVATE void game_add_input(u8 input) {
	game_context.active_sequence.buffer[game_context.active_position] = input;
	game_context.active_position += 1;
}

PRIVATE void game_prepare_update(f32 delta) {
	game_context.elapsed += delta;
	if (game_context.elapsed >= 1.0f) {
		game_set_phase(GAME_PHASE_INPUT);
	}
}

PRIVATE void game_input_update(f32 delta) {
	game_context.elapsed += delta;
	if (game_context.elapsed >= game_context.input_times[game_context.input_time_position]) {
		game_set_phase(GAME_PHASE_ATTACK_PLAYER);
	} else {
		if (game_context.active_position < SEQUENCE_MAX_INPUT && game_context.attack_count < MAX_ATTACK_PER_TURN) {
			if (IsKeyPressed(KEY_W)) {
				game_add_input(1);
				PlaySound(resources_click_1_sound);
			} else if (IsKeyPressed(KEY_D)) {
				game_add_input(2);
				PlaySound(resources_click_2_sound);
			} else if (IsKeyPressed(KEY_S)) {
				game_add_input(3);
				PlaySound(resources_click_3_sound);
			} else if (IsKeyPressed(KEY_A)) {
				game_add_input(4);
				PlaySound(resources_click_4_sound);
			}
		}

		if (IsKeyPressed(KEY_SPACE)) {
			u8 idx = 0;
			if (game_try_get_sequence_idx(&game_context.active_sequence, &idx)) {
				if (idx == 0) {
					PlaySound(resources_error_5_sound);
				}
			} else {
				PlaySound(resources_error_5_sound);
			}
			game_context.attack_queue[game_context.attack_count] = idx;
			game_context.attack_count += 1;

			memset(&game_context.active_sequence, 0, sizeof(Sequence));
			game_context.active_position = 0;
		}
	}
}

PUBLIC void game_attack_player_update(GameContext *context, f32 delta) {
	context->elapsed += delta;
	if (context->elapsed >= 0.5f) {
		if (context->attack_position >= context->attack_count) {
			game_set_phase(GAME_PHASE_ATTACK_ENEMY);
			return;
		}

		const StageInfo *stage_info = &context->stage_infos[context->stage];
		const AttackInfo *attack_info = &attack_infos[context->attack_queue[context->attack_position]];

		switch (attack_info->type) {
		case ATTACK_TYPE_SINGLE: {
			u8 idx = 0;
			for (u32 i = 0; i < stage_info->data.battle_data.enemies_len; i++) {
				if (context->enemy_healths[i] > 0) {
					idx = i;
					break;
				}
			}

			if (context->enemy_healths[idx] >= attack_info->damage) {
				context->enemy_healths[idx] -= attack_info->damage;
			} else {
				context->enemy_healths[idx] = 0;
			}
		} break;
		case ATTACK_TYPE_AOE: {
			for (u32 i = 0; i < stage_info->data.battle_data.enemies_len; i++) {
				if (context->enemy_healths[i] >= attack_info->damage) {
					context->enemy_healths[i] -= attack_info->damage;
				} else {
					context->enemy_healths[i] = 0;
				}
			}
		} break;
		case ATTACK_TYPE_SPLASH: {
			u16 applied = 0;
			for (u32 i = 0; i < stage_info->data.battle_data.enemies_len; i++) {
				if (context->enemy_healths[i] > 0) {
					u16 remainder = attack_info->damage - applied;
					if (context->enemy_healths[i] >= remainder) {
						applied += remainder;
						context->enemy_healths[i] -= remainder;
					} else {
						applied += context->enemy_healths[i];
						context->enemy_healths[i] = 0;
					}

					if (applied >= attack_info->damage) {
						break;
					}
				}
			}
		} break;
		}

		TraceLog(LOG_INFO, "player %s - %u", attack_info->name, attack_info->damage);
		context->attack_position += 1;
		context->elapsed = 0.0f;
		PlaySound(resources_error_5_sound);
	}
}

PRIVATE void game_attack_enemy_update(GameContext *context, f32 delta) {
	const StageInfo *stage_info = &context->stage_infos[context->stage];
	while (context->enemy_healths[context->enemy_attack_position] == 0 && context->enemy_attack_position < stage_info->data.battle_data.enemies_len) {
		context->enemy_attack_position += 1;
	}

	context->elapsed += delta;
	if (context->elapsed >= 0.5f) {
		if (context->enemy_attack_position >= stage_info->data.battle_data.enemies_len) {
			game_set_phase(GAME_PHASE_CHECK);
			return;
		}

		if (context->enemy_healths[context->enemy_attack_position] > 0) {
			u8 enemy_id = stage_info->data.battle_data.enemy_ids[context->enemy_attack_position];
			const EnemyInfo *enemy_info = &enemy_infos[enemy_id];
			const EnemyAttackInfo *info = &enemy_attack_infos[enemy_infos[enemy_id].attack_id];
			TraceLog(LOG_INFO, "(%d) %s: %s - %u", context->enemy_attack_position, enemy_info->name, info->name, info->damage);

			if (context->player_health >= info->damage) {
				context->player_health -= info->damage;
			} else {
				context->player_health = 0;
			}
		}

		PlaySound(resources_error_5_sound);
		context->enemy_attack_position += 1;
		context->elapsed = 0.0f;
	}
}

PRIVATE void game_advance_stage(GameContext *context) {
	context->player_health += 5;
	if (context->player_health > MAX_PLAYER_HEALTH) {
		context->player_health = MAX_PLAYER_HEALTH;
	}

	context->stage += 1;
	if (context->stage < context->stage_infos_len) {
		game_transition_stage_type(context);
	} else {
		scene_set_scene(SCENE_ENDING);
	}
}

PRIVATE void game_check_update(GameContext *context) {
	if (context->player_health == 0) {
		scene_set_scene(SCENE_GAME_OVER);
	} else {
		const StageInfo *stage_info = &context->stage_infos[context->stage];
		b8 done = true;
		for (u32 i = 0; i < stage_info->data.battle_data.enemies_len; i++) {
			if (context->enemy_healths[i] > 0) {
				done = false;
			}
		}

		if (done) {
			game_advance_stage(context);
		} else {
			game_set_phase(GAME_PHASE_PREPARE);
		}
	}
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
		switch (game_context.phase) {
		case GAME_PHASE_PREPARE: {
			game_prepare_update(delta);
		} break;
		case GAME_PHASE_INPUT: {
			game_input_update(delta);
		} break;
		case GAME_PHASE_ATTACK_PLAYER: {
			game_attack_player_update(&game_context, delta);
		} break;
		case GAME_PHASE_ATTACK_ENEMY: {
			game_attack_enemy_update(&game_context, delta);
		} break;
		case GAME_PHASE_CHECK: {
			game_check_update(&game_context);
		} break;
		case GAME_PHASE_GRIMOIRE_WAIT: {
			game_context.elapsed += delta;
			if (game_context.elapsed > 1.5f) {
				game_set_phase(GAME_PHASE_GRIMOIRE_CONTINUE);
			}
		} break;
		case GAME_PHASE_GRIMOIRE_CONTINUE: {
			if (IsKeyPressed(KEY_SPACE)) {
				game_advance_stage(&game_context);
			}
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
		const AttackInfo *attack_info = &attack_infos[context->known_attacks[i]];
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
		const AttackInfo *attack_info = &attack_infos[context->attack_queue[context->attack_count - 1]];
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
		gameplay_render_current_attack(&game_context, &attack_infos[game_context.attack_queue[game_context.attack_position]]);
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
		const AttackInfo *attack_info = &attack_infos[stage_info->data.grimoire_data.attack_id];
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
