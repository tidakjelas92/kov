#define SEQUENCE_MAX_INPUT 8

// 0: up, 1: right, 2: down, 3: left
typedef struct Sequence {
	u8 buffer[SEQUENCE_MAX_INPUT];
} Sequence;

PUBLIC u8 sequence_get_len(const Sequence *s) {
	u8 len = 0;
	for (u32 i = 0; i < SEQUENCE_MAX_INPUT; i++) {
		if (s->buffer[i] == 0) {
			break;
		} else {
			len += 1;
		}
	}

	return len;
}

PUBLIC b8 sequence_compare(const Sequence *a, const Sequence *b) {
	b8 result = false;
	for (u32 i = 0; i < SEQUENCE_MAX_INPUT; i++) {
		if (a->buffer[i] == 0 && b->buffer[i] == 0) {
			result = true;
			break;
		} else if (a->buffer[i] != b->buffer[i]) {
			break;
		}
	}

	return result;
}
