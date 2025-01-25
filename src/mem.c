#ifndef MEM_DEFAULT_ALIGNMENT
#define MEM_DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

PUBLIC inline b8 is_power_of_two(uptr x) { return (x & (x - 1)) == 0; }

PRIVATE uptr mem_align_forward_uptr(uptr ptr, u64 align) {
	assert(is_power_of_two(align) && "align must be power of 2");

	uptr p = ptr;
	uptr a = (uptr)align;
	// Same as (p % a)  but faster as 'a' is a power of two
	uptr modulo = p & (a - 1);

	if (modulo != 0) {
		// if 'p' address is not aligned, push the address to the next value which
		// is aligned
		p += a - modulo;
	}

	return p;
}

typedef struct MemArena {
	void *buffer;
	u64 len;
	u64 used;
} MemArena;

PUBLIC void mem_arena_init(MemArena *a, void *buffer, u64 len) {
	assert(buffer != NULL);
	assert(len > 0);
	a->buffer = buffer;
	a->len = len;
	a->used = 0;
}

PUBLIC void *mem_arena_alloc_align(MemArena *a, u64 size, u64 align) {
	assert(size > 0);

	uptr current = (uptr)a->buffer + (uptr)a->used;
	uptr used = mem_align_forward_uptr(current, align);
	used -= (uptr)a->buffer; // change to relative used.

	// assert compiles to nothing on NDEBUG
	if (used + size > a->len) {
		assert(false && "arena is full.");
		return NULL;
	}

	void *ptr = &a->buffer[used];
	a->used = used + size;

	return ptr;
}

#define mem_arena_alloc(a, size) mem_arena_alloc_align(a, size, MEM_DEFAULT_ALIGNMENT)
