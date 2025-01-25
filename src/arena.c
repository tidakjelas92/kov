typedef struct Arena {
	void *buffer;
	u64 len;
	u64 offset;
} Arena;

void *arena_alloc_align(Arena *a, u64 size, u64 align) {
	uptr current = (uptr)a->buffer + (uptr)a->offset;
	uptr offset = _align_forward_uptr(current, align);
	offset -= (uptr)a->buffer; // change to relative offset.

	// assert compiles to nothing on NDEBUG
	if (offset + size > a->len) {
		assert(false && "arena is full.");
		return NULL;
	}

	void *ptr = &a->buffer[offset];
	a->offset = offset + size;

	return ptr;
}

#define arena_alloc(a, size) arena_alloc_align(a, size, DEFAULT_ALIGNMENT)

void arena_reset(Arena *a) {
	assert(a->offset > 0);
	a->offset = 0;
}
