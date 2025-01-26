#define MAX_INPUT_PER_SEQUENCE 8
#define MAX_ATTACK_PER_TURN 32
#define MAX_ENEMIES_PER_STAGE 5
#define MAX_PLAYER_HEALTH 40

// 0: up, 1: right, 2: down, 3: left
typedef struct Sequence {
	u8 buffer[MAX_INPUT_PER_SEQUENCE];
} Sequence;

typedef enum GamePhase {
	GAME_PHASE_START,
	GAME_PHASE_PREPARE,
	GAME_PHASE_INPUT,
	GAME_PHASE_ATTACK,
	GAME_PHASE_CHECK,
	GAME_PHASE_GRIMOIRE_WAIT,
	GAME_PHASE_GRIMOIRE_CONTINUE,
} GamePhase;

// these happen in GAME_PHASE_ATTACK
typedef enum GameAttackPhaseStep {
	GAME_ATTACK_PHASE_STEP_START,
	GAME_ATTACK_PHASE_STEP_PLAYER,
	GAME_ATTACK_PHASE_STEP_ENEMY,
	GAME_ATTACK_PHASE_STEP_DONE
} GameAttackPhaseStep;

typedef enum AttackType {
	ATTACK_TYPE_SINGLE,
	ATTACK_TYPE_AOE,
	ATTACK_TYPE_SPLASH
} AttackType;

typedef enum StageType {
	STAGE_TYPE_BATTLE,
	STAGE_TYPE_GRIMOIRE
} StageType;

typedef struct StageInfo {
	StageType type;
	union {
		struct {
			u8 enemy_ids[MAX_ENEMIES_PER_STAGE];
			u8 enemies_len;
		} battle_data;
		struct {
			u8 attack_id;
		} grimoire_data;
	} data;
} StageInfo;

typedef struct AttackInfo {
	const char *name;
	Sequence sequence;
	AttackType type;
	u16 damage;
} AttackInfo;

GLOBAL const AttackInfo attack_infos[] = {
	{ "??", { 0, 0, 0, 0, 0, 0, 0, 0 }, ATTACK_TYPE_SINGLE, 0 },
	{ "Slash", { 1, 2, 2, 0, 0, 0, 0, 0 }, ATTACK_TYPE_SINGLE, 5 },
	{ "Cross Slash", { 1, 2, 2, 1, 4, 4, 0, 0 }, ATTACK_TYPE_SINGLE, 12 },
	{ "Twirl", { 2, 3, 4, 2, 0, 0, 0, 0 }, ATTACK_TYPE_AOE, 4 },
	{ "Spear Thrust", { 2, 2, 2, 2, 2, 2, 2, 2 }, ATTACK_TYPE_SPLASH, 15 }
};
#define ATTACK_INFOS_COUNT sizeof(attack_infos) / sizeof(AttackInfo)

typedef struct GameContext {
	u8 attack_queue[MAX_ATTACK_PER_TURN];
	Sequence active_sequence;

	const f32 *input_times;
	const StageInfo *stage_infos;

	u8 known_attacks[ATTACK_INFOS_COUNT];
	u16 enemy_healths[MAX_ENEMIES_PER_STAGE];

	f32 elapsed;
	u16 player_health;
	GamePhase phase;
	GameAttackPhaseStep attack_phase_step;

	u8 input_time_position;
	u8 input_times_len;

	u8 active_position;
	u8 attack_count;
	u8 attack_position;
	u8 enemy_attack_position;

	u8 stage_infos_len;
	u8 stage;

	u8 known_attacks_len;
} GameContext;

// TODO: allocate context with gameplay arena.
GLOBAL GameContext game_context;
GLOBAL b8 paused;

// TODO: separate game data to own file.
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
	{ "Swarm", 1, 3, ONEBIT_TILE_SWARM },
	{ "Arachnid Queen", 30, 3, ONEBIT_TILE_ARACHNID_QUEEN },
	{ "Living Armor", 20, 3, ONEBIT_TILE_LIVING_ARMOR },
	{ "Demon", 50, 5, ONEBIT_TILE_DEMON }
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


PRIVATE void game_prepare_enemies(GameContext *context) {
	const StageInfo *stage_info = &context->stage_infos[context->stage];
	for (u32 i = 0; i < stage_info->data.battle_data.enemies_len; i++) {
		const EnemyInfo *enemy_info = &enemy_infos[stage_info->data.battle_data.enemy_ids[i]];
		context->enemy_healths[i] = enemy_info->health;
	}
}

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
	case GAME_PHASE_ATTACK: {
		game_context.elapsed = 0.0f;
		game_context.attack_position = 0;
		game_context.enemy_attack_position = 0;
		game_context.attack_phase_step = GAME_ATTACK_PHASE_STEP_START;
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
		game_prepare_enemies(context);
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

	paused = false;

	game_transition_stage_type(&game_context);
}

PRIVATE u8 sequence_get_len(const Sequence *s) {
	u8 len = 0;
	for (u32 i = 0; i < MAX_INPUT_PER_SEQUENCE; i++) {
		if (s->buffer[i] == 0) {
			break;
		} else {
			len += 1;
		}
	}

	return len;
}

PRIVATE b8 sequence_compare(const Sequence *a, const Sequence *b) {
	for (u32 i = 0; i < MAX_INPUT_PER_SEQUENCE; i++) {
		if (a->buffer[i] == 0 && b->buffer[i] == 0) {
			return true;
		} else if (a->buffer[i] != b->buffer[i]) {
			return false;
		}
	}

	return true;
}

PRIVATE b8 sequence_try_get_sequence_idx(const Sequence *seq, u8 *result) {
	b8 found = false;
	for (u32 i = 0; i < ATTACK_INFOS_COUNT; i++) {
		if (sequence_compare(&attack_infos[i].sequence, seq)) {
			*result = i;
			found = true;
			break;
		}
	}

	return found;
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
		game_set_phase(GAME_PHASE_ATTACK);
	} else {
		if (game_context.active_position < MAX_INPUT_PER_SEQUENCE && game_context.attack_count < MAX_ATTACK_PER_TURN) {
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
			if (sequence_try_get_sequence_idx(&game_context.active_sequence, &idx)) {
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

PUBLIC void game_attack_update(f32 delta) {
	const StageInfo *stage_info = &game_context.stage_infos[game_context.stage];

	switch (game_context.attack_phase_step) {
	case GAME_ATTACK_PHASE_STEP_START: {
		game_context.attack_phase_step = GAME_ATTACK_PHASE_STEP_PLAYER;
	} break;
	case GAME_ATTACK_PHASE_STEP_PLAYER: {
		game_context.elapsed += delta;
		if (game_context.elapsed >= 0.5f) {
			const AttackInfo *attack_info = &attack_infos[game_context.attack_queue[game_context.attack_position]];

			switch (attack_info->type) {
			case ATTACK_TYPE_SINGLE: {
				u8 idx = 0;
				for (u32 i = 0; i < stage_info->data.battle_data.enemies_len; i++) {
					if (game_context.enemy_healths[i] > 0) {
						idx = i;
						break;
					}
				}

				if (game_context.enemy_healths[idx] >= attack_info->damage) {
					game_context.enemy_healths[idx] -= attack_info->damage;
				} else {
					game_context.enemy_healths[idx] = 0;
				}
			} break;
			case ATTACK_TYPE_AOE: {
				for (u32 i = 0; i < stage_info->data.battle_data.enemies_len; i++) {
					if (game_context.enemy_healths[i] >= attack_info->damage) {
						game_context.enemy_healths[i] -= attack_info->damage;
					} else {
						game_context.enemy_healths[i] = 0;
					}
				}
			} break;
			case ATTACK_TYPE_SPLASH: {
				u16 applied = 0;
				for (u32 i = 0; i < stage_info->data.battle_data.enemies_len; i++) {
					if (game_context.enemy_healths[i] > 0) {
						u16 remainder = attack_info->damage - applied;
						if (game_context.enemy_healths[i] >= remainder) {
							applied += remainder;
							game_context.enemy_healths[i] -= remainder;
						} else {
							applied += game_context.enemy_healths[i];
							game_context.enemy_healths[i] = 0;
						}

						if (applied >= attack_info->damage) {
							break;
						}
					}
				}
			} break;
			}

			TraceLog(LOG_INFO, "player %s - %u", attack_info->name, attack_info->damage);
			game_context.attack_position += 1;
			game_context.elapsed = 0.0f;
			if (game_context.attack_position >= game_context.attack_count) {
				game_context.attack_phase_step = GAME_ATTACK_PHASE_STEP_ENEMY;
			}
		}
	} break;
	case GAME_ATTACK_PHASE_STEP_ENEMY: {
		game_context.elapsed += delta;
		if (game_context.elapsed >= 0.5f) {
			while (game_context.enemy_healths[game_context.enemy_attack_position] == 0 && game_context.enemy_attack_position + 1 < stage_info->data.battle_data.enemies_len) {
				game_context.enemy_attack_position += 1;
			}

			if (game_context.enemy_healths[game_context.enemy_attack_position] > 0) {
				u8 enemy_id = stage_info->data.battle_data.enemy_ids[game_context.enemy_attack_position];
				const EnemyInfo *enemy_info = &enemy_infos[enemy_id];
				const EnemyAttackInfo *info = &enemy_attack_infos[enemy_infos[enemy_id].attack_id];
				TraceLog(LOG_INFO, "(%d) %s: %s - %u", game_context.enemy_attack_position, enemy_info->name, info->name, info->damage);

				if (game_context.player_health >= info->damage) {
					game_context.player_health -= info->damage;
				} else {
					game_context.player_health = 0;
				}
			}

			game_context.enemy_attack_position += 1;
			game_context.elapsed = 0.0f;
			if (game_context.enemy_attack_position >= stage_info->data.battle_data.enemies_len) {
				game_context.attack_phase_step = GAME_ATTACK_PHASE_STEP_DONE;
			}
		}
	} break;
	case GAME_ATTACK_PHASE_STEP_DONE: {
		game_set_phase(GAME_PHASE_CHECK);
	} break;
	default: {
	} break;
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
		paused = !paused;
		PlaySound(resources_switch_2_sound);
	}

	if (paused) {
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
		case GAME_PHASE_ATTACK: {
			game_attack_update(delta);
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
		position.y += row_height;

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

	Vector2 bars_start_position = { GetScreenWidth() / 2.0f + 300.0f, 250.0f };
	for (u32 i = 0; i < stage_info->data.battle_data.enemies_len; i++) {
		bars_start_position.y += 25.0f;
		ui_draw_health_bar(
			bars_start_position,
			160,
			(f32)game_context.enemy_healths[i] / (f32)enemy_infos[stage_info->data.battle_data.enemy_ids[i]].health
		);
	}
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
		(Vector2){ 100, 250 },
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
		Vector2 arrow_dst_size = { 48.0f, 48.0f };
		Vector2 start_position = {
			GetScreenWidth() / 2.0f - arrow_dst_size.x * game_context.active_position / 2.0f,
			GetScreenHeight() - 120.0f - arrow_dst_size.y
		};

		for (u32 i = 0; i < game_context.active_position; i++) {
			Rectangle arrow_dst_rect = {
				start_position.x + arrow_dst_size.x * i,
				start_position.y,
				arrow_dst_size.x,
				arrow_dst_size.y
			};
			DrawTexturePro(
				resources_prompt_texture,
				// basically works because the value is literally the same.
				prompt_tiles[game_context.active_sequence.buffer[i] - 1],
				arrow_dst_rect,
				Vector2Zero(), 0.0f,
				THEME_BLACK
			);
		}

		for (u32 i = 0; i < game_context.attack_count; i++) {
			f32 start_y = 300.0f;

			const char *text = attack_infos[game_context.attack_queue[i]].name;
			Vector2 text_size = MeasureTextEx(resources_pixel_operator_font, text, resources_pixel_operator_font.baseSize, 2.0f);
			DrawTextEx(
				resources_pixel_operator_font,
				text,
				(Vector2){
					GetScreenWidth() / 2.0f - text_size.x / 2.0f,
					start_y + text_size.y * i,
				},
				resources_pixel_operator_font.baseSize, 0.0f,
				THEME_BLACK
			);
		}

		if (!paused) {
			ui_draw_space_confirm(THEME_BLACK);
		}

		f32 input_time = game_context.input_times[game_context.input_time_position];
		f32 time_bar_width = (input_time - game_context.elapsed) / input_time * GetScreenWidth();
		DrawRectangle(
			GetScreenWidth() / 2.0f - time_bar_width / 2.0f,
			GetScreenHeight() - 10,
			time_bar_width,
			10,
			THEME_BLACK
		);
	} break;
	case GAME_PHASE_GRIMOIRE_CONTINUE: {
		if (!paused) {
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

	if (paused) {
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
