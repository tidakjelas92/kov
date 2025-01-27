#define MAX_ATTACK_PER_TURN 32
#define MAX_ENEMIES_PER_STAGE 5
#define MAX_PLAYER_HEALTH 40

typedef enum GamePhase {
	GAME_PHASE_START,
	GAME_PHASE_PREPARE,
	GAME_PHASE_INPUT,
	GAME_PHASE_ATTACK_PLAYER,
	GAME_PHASE_ATTACK_ENEMY,
	GAME_PHASE_CHECK,
	GAME_PHASE_GRIMOIRE_WAIT,
	GAME_PHASE_GRIMOIRE_CONTINUE,
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

// TODO: reference this in game context like stage_infos.
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

PRIVATE b8 game_try_get_sequence_idx(const Sequence *seq, u8 *result) {
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
