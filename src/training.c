#define MAX_INPUT_PER_SEQUENCE 8
#define MAX_ATTACK_PER_TURN 32

typedef struct Sequence {
	u8 buffer[MAX_INPUT_PER_SEQUENCE];
} Sequence;

typedef struct Health {
	u16 max;
	u16 value;
} Health;

typedef struct Enemy {
	Health health;
	// per turn, does this attack
	u8 attack_id;
} Enemy;

typedef enum GamePhase {
	GAME_PHASE_PREPARE,
	GAME_PHASE_INPUT,
	GAME_PHASE_ATTACK,
	GAME_PHASE_CHECK
} GamePhase;

typedef struct GameContext {
	u8 attack_queue[MAX_ATTACK_PER_TURN];
	u8 enemy_attack_queue[8];
	f32 input_time[4];
	Sequence active_sequence;

	Enemy *enemies;
	u64 enemies_len;

	u64 stage;

	f32 elapsed;
	Health player_health;
	GamePhase phase;

	u8 active_position;
	u8 input_time_position;
	u8 attack_count;
	u8 attack_position;
	u8 enemy_attack_count;
	u8 enemy_attack_position;
} GameContext;

GLOBAL GameContext game_context;

typedef struct AttackInfo {
	const char *name;
	Sequence sequence;
	u16 damage;
} AttackInfo;

GLOBAL AttackInfo attack_infos[] = {
	{ "??", { 0, 0, 0, 0, 0, 0, 0, 0 }, 0 },
	{ "Slash", { 1, 2, 2, 0, 0, 0, 0, 0 }, 5 },
	{ "Cross Slash", { 1, 2, 2, 1, 4, 4, 0, 0 }, 12 },
	{ "Twirl", { 2, 3, 4, 2, 0, 0, 0, 0 }, 4 },
	{ "Spear Thrust", { 2, 2, 2, 2, 2, 2, 2, 2 }, 15 }
};
#define ATTACK_INFOS_COUNT sizeof(attack_infos) / sizeof(AttackInfo)

typedef struct EnemyAttackInfo {
	const char *name;
	u16 damage;
} EnemyAttackInfo;

GLOBAL EnemyAttackInfo enemy_attack_infos[] = {
	{ "Stare", 0 },
	{ "Hit", 2 },
	{ "Scratch", 2 },
	{ "Bite", 3 }
};
#define ENEMY_ATTACK_INFOS_COUNT sizeof(enemy_attack_infos) / sizeof(EnemyAttackInfo)

GLOBAL Enemy training_enemies[] = {
	{ { 10, 10 }, 0 },
	{ { 15, 15 }, 0 },
	{ { 30, 30 }, 1 }
};
#define TRAINING_ENEMIES_LEN sizeof(training_enemies) / sizeof(Enemy)


PUBLIC void game_set_phase(GamePhase phase) {
	if (game_context.phase == phase) {
		TraceLog(LOG_WARNING, "changing phase to the same phase.");
		return;
	}

	// on exit
	switch (game_context.phase) {
	case GAME_PHASE_INPUT: {
		// TODO: queue up enemy attacks
		game_context.enemy_attack_queue[0] = game_context.enemies[game_context.stage].attack_id;
		game_context.enemy_attack_count = 1;

		// scroll through input_time
		game_context.input_time_position += 1;
		if (game_context.input_time_position >= sizeof(game_context.input_time) / sizeof(f32)) {
			game_context.input_time_position = 0;
		}
	} break;
	default: {
	} break;
	}
	game_context.phase = phase;

	// on enter
	switch (game_context.phase) {
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
	} break;
	default: {
	} break;
	}
}

PUBLIC void training_init(void) {
	TraceLog(LOG_INFO, "sizeof GamePhase: %zu", sizeof(GamePhase));
	TraceLog(LOG_INFO, "sizeof attack_infos: %zu", sizeof(attack_infos));
	TraceLog(LOG_INFO, "sizeof GameContext: %zu", sizeof(GameContext));
	memset(&game_context, 0, sizeof(GameContext));
	game_context.input_time[0] = 4.0f;
	game_context.input_time[1] = 3.5f;
	game_context.input_time[2] = 1.5f;
	game_context.input_time[3] = 2.5f;
	game_context.player_health.max = 20;
	game_context.player_health.value = 20;
	game_context.enemies = training_enemies;
	game_context.enemies_len = TRAINING_ENEMIES_LEN;
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

PRIVATE void game_add_damage(Health *health, u32 damage) {
	if (health->value >= damage) {
		health->value -= damage;
	} else {
		health->value = 0;
	}
}

PUBLIC void training_update(f32 delta) {
	switch (game_context.phase) {
	case GAME_PHASE_PREPARE: {
		game_context.elapsed += delta;
		if (game_context.elapsed >= 1.0f) {
			game_set_phase(GAME_PHASE_INPUT);
		}
	} break;
	case GAME_PHASE_INPUT: {
		game_context.elapsed += delta;
		if (game_context.elapsed >= game_context.input_time[game_context.input_time_position]) {
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
	} break;
	case GAME_PHASE_ATTACK: {
		if (game_context.attack_position < game_context.attack_count) {
			game_context.elapsed += delta;
			if (game_context.elapsed >= 0.5f) {
				AttackInfo *info = &attack_infos[game_context.attack_queue[game_context.attack_position]];

				Enemy *enemy = &game_context.enemies[game_context.stage];
				game_add_damage(&enemy->health, info->damage);

				TraceLog(LOG_INFO, "%s - %u", info->name, info->damage);
				game_context.attack_position += 1;
				game_context.elapsed = 0.0f;
			}
		} else if (game_context.enemy_attack_position < game_context.enemy_attack_count) {
			game_context.elapsed += delta;
			if (game_context.elapsed >= 0.5f) {
				EnemyAttackInfo *info = &enemy_attack_infos[game_context.enemy_attack_queue[game_context.enemy_attack_position]];
				TraceLog(LOG_INFO, "%s - %u", info->name, info->damage);
				game_add_damage(&game_context.player_health, info->damage);
				game_context.enemy_attack_position += 1;
				game_context.elapsed = 0.0f;
			}
		} else {
			game_set_phase(GAME_PHASE_CHECK);
		}
	} break;
	case GAME_PHASE_CHECK: {
		// TODO: check if player lose or win
		if (game_context.player_health.value == 0) {
			// TODO: lose
		} else if (game_context.enemies[game_context.stage].health.value == 0) {
			game_context.stage += 1;
			if (game_context.stage < game_context.enemies_len) {
				game_set_phase(GAME_PHASE_PREPARE);
			} else {
				scene_set_scene(SCENE_ENDING);
			}
		} else {
			game_set_phase(GAME_PHASE_PREPARE);
		}
	} break;
	}
}

PUBLIC void training_render(void) {
	char stage_text[6];
	snprintf(stage_text, sizeof(stage_text),"%zu/%zu", game_context.stage, game_context.enemies_len);
	Vector2 stage_text_size = MeasureTextEx(resources_pixelplay_font, stage_text, resources_pixelplay_font.baseSize * 3.0f, 4.0f);
	DrawTextEx(
		resources_pixelplay_font,
		stage_text,
		(Vector2){ GetScreenWidth() / 2.0f - stage_text_size.x / 2.0f, 10.0f },
		resources_pixelplay_font.baseSize * 3.0f, 4.0f,
		THEME_BLACK
	);

	DrawTexturePro(
		resources_rpg_texture,
		(Rectangle){ 0, 112, 16, 16 },
		(Rectangle){ GetScreenWidth() / 2.0f - 300.0f, 300.0f, 32.0f, 32.0f },
		Vector2Zero(), 0.0f,
		WHITE
	);
	ui_draw_health_bar(
		(Vector2){ 100, 250 },
		160,
		(f32)game_context.player_health.value / (f32)game_context.player_health.max
	);

	Enemy *enemy = &game_context.enemies[game_context.stage];
	ui_draw_health_bar(
		(Vector2){ 600, 250 },
		160,
		(f32)enemy->health.value / (f32)enemy->health.max
	);

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
		Rectangle arrow_rects[] = {
			{ 480, 192, 16, 16 },
			{ 496, 192, 16, 16 },
			{ 512, 192, 16, 16 },
			{ 528, 192, 16, 16 },
		};
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
				arrow_rects[game_context.active_sequence.buffer[i] - 1],
				arrow_dst_rect,
				Vector2Zero(), 0.0f,
				THEME_BLACK
			);
		}

		for (u32 i = 0; i < game_context.attack_count; i++) {
			f32 start_y = 300.0f;

			const char *text = attack_infos[game_context.attack_queue[i]].name;
			Vector2 text_size = MeasureTextEx(GuiGetFont(), text, GuiGetFont().baseSize, 2.0f);
			Rectangle text_rect = {
				GetScreenWidth() / 2.0f - text_size.x / 2.0f,
				start_y + text_size.y * i,
				text_size.x,
				text_size.y
			};
			GuiLabel(text_rect, text);
		}

		ui_draw_space_confirm();

		f32 input_time = game_context.input_time[game_context.input_time_position];
		f32 time_bar_width = (input_time - game_context.elapsed) / input_time * GetScreenWidth();
		DrawRectangle(
			GetScreenWidth() / 2.0f - time_bar_width / 2.0f,
			GetScreenHeight() - 10,
			time_bar_width,
			10,
			THEME_BLACK
		);
	} break;
	}
}
