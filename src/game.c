#define MAX_ATTACK_PER_TURN 32
#define MAX_ENEMIES_PER_STAGE 5

typedef enum GamePhase {
	GAME_PHASE_START,
	GAME_PHASE_PREPARE,
	GAME_PHASE_INPUT,
	GAME_PHASE_ATTACK_PLAYER,
	GAME_PHASE_ATTACK_ENEMY,
	GAME_PHASE_CHECK,
	GAME_PHASE_GRIMOIRE_WAIT,
	GAME_PHASE_GRIMOIRE_CONTINUE,
	GAME_PHASE_LOSE,
	GAME_PHASE_WIN
} GamePhase;

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
	u8 type; // enum StageType
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
	u16 damage;
	u8 type; // enum AttackType
} AttackInfo;

typedef struct EnemyAttackInfo {
	const char *name;
	u16 damage;
} EnemyAttackInfo;

typedef struct EnemyInfo {
	const char *name;
	u16 health;
	u8 attack_id;
	u8 tile;
} EnemyInfo;

typedef struct GameContext {
	u8 attack_queue[MAX_ATTACK_PER_TURN];
	u16 enemy_healths[MAX_ENEMIES_PER_STAGE];
	Sequence active_sequence;

	const f32 *input_times;
	const StageInfo *stage_infos;
	const AttackInfo *attack_infos;
	const EnemyAttackInfo *enemy_attack_infos;
	const EnemyInfo *enemy_infos;

	// array len is not required here, so we don't store it.
	u8 *known_attacks;

	f32 elapsed;
	u16 player_health;
	u16 player_max_health;
	u8 phase; // enum GamePhase

	u8 input_time_position;
	u8 input_times_len;

	u8 active_position;
	u8 attack_count;
	u8 attack_position;
	u8 enemy_attack_position;

	u8 stage_infos_len;
	u8 stage;

	// this is not the array len! this is how many attacks are known at gameplay
	u8 known_attacks_count;
	u8 attack_infos_len;
} GameContext;

PUBLIC b8 game_try_get_sequence_idx(const GameContext *context, const Sequence *seq, u8 *result) {
	b8 found = false;
	for (u32 i = 0; i < context->attack_infos_len; i++) {
		if (sequence_compare(&context->attack_infos[i].sequence, seq)) {
			*result = i;
			found = true;
			break;
		}
	}

	return found;
}

PUBLIC void game_set_phase(GameContext *context, GamePhase phase) {
	if (context->phase == phase) {
		TraceLog(LOG_WARNING, "changing phase to the same phase.");
		return;
	}

	// on exit
	switch (context->phase) {
	case GAME_PHASE_START: {
		context->player_health = context->player_max_health;
	} break;
	case GAME_PHASE_INPUT: {
		// scroll through input_time
		context->input_time_position += 1;
		if (context->input_time_position >= context->input_times_len) {
			context->input_time_position = 0;
		}
	} break;
	default: {
	} break;
	}
	context->phase = phase;

	// on enter
	switch (context->phase) {
	case GAME_PHASE_PREPARE: {
		context->elapsed = 0.0f;
	} break;
	case GAME_PHASE_INPUT: {
		memset(&context->attack_queue, 0, MAX_ATTACK_PER_TURN);
		memset(&context->active_sequence, 0, sizeof(Sequence));
		context->active_position = 0;
		context->elapsed = 0.0f;
		context->attack_count = 0;
	} break;
	case GAME_PHASE_ATTACK_PLAYER: {
		context->elapsed = 0.0f;
		context->attack_position = 0;
	} break;
	case GAME_PHASE_ATTACK_ENEMY: {
		context->elapsed = 0.0f;
		context->enemy_attack_position = 0;
	} break;
	case GAME_PHASE_GRIMOIRE_WAIT: {
		context->elapsed = 0.0f;
	} break;
	case GAME_PHASE_GRIMOIRE_CONTINUE: {
		u8 attack_id = context->stage_infos[context->stage].data.grimoire_data.attack_id;
		context->known_attacks[context->known_attacks_count] = attack_id;
		context->known_attacks_count += 1;
	} break;
	default: {
	} break;
	}
}

PRIVATE inline void game_add_input(GameContext *context, u8 input) {
	context->active_sequence.buffer[context->active_position] = input;
	context->active_position += 1;
}

PRIVATE void game_input_update(GameContext *context, f32 delta) {
	context->elapsed += delta;
	if (context->elapsed >= context->input_times[context->input_time_position]) {
		game_set_phase(context, GAME_PHASE_ATTACK_PLAYER);
	} else {
		if (context->active_position < SEQUENCE_MAX_INPUT && context->attack_count < MAX_ATTACK_PER_TURN) {
			if (IsKeyPressed(KEY_W)) {
				game_add_input(context, 1);
				PlaySound(resources_click_1_sound);
			} else if (IsKeyPressed(KEY_D)) {
				game_add_input(context, 2);
				PlaySound(resources_click_2_sound);
			} else if (IsKeyPressed(KEY_S)) {
				game_add_input(context, 3);
				PlaySound(resources_click_3_sound);
			} else if (IsKeyPressed(KEY_A)) {
				game_add_input(context, 4);
				PlaySound(resources_click_4_sound);
			}
		}

		if (IsKeyPressed(KEY_SPACE)) {
			u8 idx = 0;
			if (game_try_get_sequence_idx(context, &context->active_sequence, &idx)) {
				if (idx == 0) {
					PlaySound(resources_error_5_sound);
				}
			} else {
				PlaySound(resources_error_5_sound);
			}
			context->attack_queue[context->attack_count] = idx;
			context->attack_count += 1;

			memset(&context->active_sequence, 0, sizeof(Sequence));
			context->active_position = 0;
		}
	}
}

PRIVATE void game_attack_player_update(GameContext *context, f32 delta) {
	context->elapsed += delta;
	if (context->elapsed >= 0.5f) {
		if (context->attack_position >= context->attack_count) {
			game_set_phase(context, GAME_PHASE_ATTACK_ENEMY);
			return;
		}

		const StageInfo *stage_info = &context->stage_infos[context->stage];
		const AttackInfo *attack_info = &context->attack_infos[context->attack_queue[context->attack_position]];

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

		TraceLog(LOG_DEBUG, "player %s - %u", attack_info->name, attack_info->damage);
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
			game_set_phase(context, GAME_PHASE_CHECK);
			return;
		}

		if (context->enemy_healths[context->enemy_attack_position] > 0) {
			u8 enemy_id = stage_info->data.battle_data.enemy_ids[context->enemy_attack_position];
			const EnemyInfo *enemy_info = &context->enemy_infos[enemy_id];
			const EnemyAttackInfo *info = &context->enemy_attack_infos[context->enemy_infos[enemy_id].attack_id];
			TraceLog(LOG_DEBUG, "(%d) %s: %s - %u", context->enemy_attack_position, enemy_info->name, info->name, info->damage);

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

PRIVATE void game_transition_stage_type(GameContext *context) {
	const StageInfo *stage_info = &context->stage_infos[context->stage];
	switch (stage_info->type) {
	case STAGE_TYPE_BATTLE: {
		game_set_phase(context, GAME_PHASE_PREPARE);
		// prepare the enemies' health values
		const StageInfo *stage_info = &context->stage_infos[context->stage];
		for (u32 i = 0; i < stage_info->data.battle_data.enemies_len; i++) {
			const EnemyInfo *enemy_info = &context->enemy_infos[stage_info->data.battle_data.enemy_ids[i]];
			context->enemy_healths[i] = enemy_info->health;
		}
	} break;
	case STAGE_TYPE_GRIMOIRE: {
		game_set_phase(context, GAME_PHASE_GRIMOIRE_WAIT);
	} break;
	}
}

PRIVATE void game_advance_stage(GameContext *context) {
	context->player_health += 5;
	if (context->player_health > context->player_max_health) {
		context->player_health = context->player_max_health;
	}

	context->stage += 1;
	if (context->stage < context->stage_infos_len) {
		game_transition_stage_type(context);
	} else {
		game_set_phase(context, GAME_PHASE_WIN);
	}
}

PRIVATE void game_check_update(GameContext *context) {
	if (context->player_health == 0) {
		game_set_phase(context, GAME_PHASE_LOSE);
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
			game_set_phase(context, GAME_PHASE_PREPARE);
		}
	}
}

PUBLIC void game_update(GameContext *context, f32 delta) {
	switch (context->phase) {
	case GAME_PHASE_PREPARE: {
		context->elapsed += delta;
		if (context->elapsed >= 1.0f) {
			game_set_phase(context, GAME_PHASE_INPUT);
		}
	} break;
	case GAME_PHASE_INPUT: {
		game_input_update(context, delta);
	} break;
	case GAME_PHASE_ATTACK_PLAYER: {
		game_attack_player_update(context, delta);
	} break;
	case GAME_PHASE_ATTACK_ENEMY: {
		game_attack_enemy_update(context, delta);
	} break;
	case GAME_PHASE_CHECK: {
		game_check_update(context);
	} break;
	case GAME_PHASE_GRIMOIRE_WAIT: {
		context->elapsed += delta;
		if (context->elapsed > 1.5f) {
			game_set_phase(context, GAME_PHASE_GRIMOIRE_CONTINUE);
		}
	} break;
	case GAME_PHASE_GRIMOIRE_CONTINUE: {
		if (IsKeyPressed(KEY_SPACE)) {
			game_advance_stage(context);
		}
	} break;
	default: {
	} break;
	}
}
