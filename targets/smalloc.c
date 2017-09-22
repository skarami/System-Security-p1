#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* glibc's malloc hooks. Silence warnings about deprecated hooks (see
 * bottom of this file). */
#undef __attribute_deprecated__
#define __attribute_deprecated__
#include <malloc.h>

/* Malloc's view of memory is in terms of chunks. The size of a chunk
 * is implicit in the difference between the right pointer and itself.
 * User memory comes after right */
struct chunk
{
	struct chunk *left;
	struct chunk *right;
};

#define TO_CHUNK(p) ((struct chunk *)(p) - 1)
#define FROM_CHUNK(c) ((void *)((c) + 1))
#define IS_IN_USE(c) ((uintptr_t)(c)->right & 1)
#define SET_IN_USE(c) ((c)->right = (struct chunk *)((uintptr_t)(c)->right | 1))
#define CLEAR_IN_USE(c) ((c)->right = (struct chunk *)((uintptr_t)(c)->right & ~1))
#define RIGHT(c) ((struct chunk *)((uintptr_t)(c)->right & ~1))
#define TO_CHUNK_SIZE(size) \
	(sizeof(struct chunk)*(((size)+sizeof(struct chunk)-1)/sizeof(struct chunk)) \
	 + sizeof(struct chunk))
#define CHUNK_SIZE(c) ((uintptr_t)RIGHT(c) -(uintptr_t)(c))

/* Some systems define sbrk(int), others sbrk(intptr_t). Just go with
 * the minimum. */
#define MAX_ALLOCATION_SIZE (INT_MAX < INTPTR_MAX? INT_MAX:INTPTR_MAX)

static int page_size;
static struct chunk *bottom;
static struct chunk *top;

/* Memory is laid out like
 * +----------+
 * |left/right|
 * +----------+ <-- top
 * |          |
 * |          |
 *     ...
 * |          |
 * |          |
 * +----------+
 * |          |
 * |   user   |
 * |          |
 * |left/right|
 * +----------+ <-- bottom
 *
 * Note that top doesn't actually point at the top of the heap, but just below
 * it.
 */

static int smalloc_init()
{
	page_size = getpagesize();
	bottom = sbrk(4*page_size);

	if ((intptr_t)bottom == -1)
	{
		bottom = 0;
		return -1;
	}
	top = (struct chunk *)((char *)bottom + 4*page_size) - 1;
	bottom->left = 0;
	bottom->right = top;
	top->left = bottom;
	top->right = 0;
	return 0;
}

static void *allocate_chunk(struct chunk *c, size_t nb)
{
	size_t chunk_size = CHUNK_SIZE(c);
	if (chunk_size > nb)
	{
		/* Carve off a new chunk. */
		struct chunk *new_chunk = (struct chunk *)((char *)c + nb);
		new_chunk->right = RIGHT(c);
		new_chunk->left = c;
		RIGHT(c)->left = new_chunk;
		c->right = new_chunk;
	}

	SET_IN_USE(c);
	return FROM_CHUNK(c);
}

void *smalloc(size_t size)
{
	if (!bottom && smalloc_init())
		goto error;

	size_t nb = TO_CHUNK_SIZE(size);

	if (nb < size)
		goto error;

	/* Find the first chunk large enough to handle the allocation. */
	struct chunk *c;
	for (c = bottom; c != top; c = RIGHT(c))
	{
		size_t chunk_size = CHUNK_SIZE(c);
		if (!IS_IN_USE(c) && chunk_size >= nb)
			return allocate_chunk(c, nb);
	}

	/* No chunks are large enough. Try to grow the heap. */
	size_t extra;
	if (IS_IN_USE(top->left))
	{
		c = top;
		extra = nb;
	}
	else
	{
		c = top->left;
		extra = nb - CHUNK_SIZE(c);
	}

	/* Allocate at least 4 pages at a time. */
	if (extra < 4*page_size)
		extra = 4*page_size;
	else
		extra = page_size * ((extra + page_size - 1)/page_size);
	
	if (extra > MAX_ALLOCATION_SIZE)
		goto error;

	void *p = sbrk(extra);
	if (p != top + 1)
		goto error;

	top = (struct chunk *)((char *)p + extra) - 1;
	top->left = c;
	top->right = 0;
	c->right = top;
	return allocate_chunk(c, nb);
error:
	errno = ENOMEM;
	return 0;
}

void *scalloc(size_t num, size_t size)
{
	size_t total = num * size;
	if ((total < num  && size != 0) || (total < size && num != 0))
	{
		errno = ENOMEM;
		return 0;
	}
	void *p = smalloc(total);
	if (p)
		memset(p, 0, total);
	return p;
}

static void deallocate_chunk(struct chunk *c)
{
	struct chunk *left = c->left;
	struct chunk *right = c->right;
	right->left = left;
	left->right = right;
}

void sfree(void *p)
{
	if (p == 0)
		return;
	struct chunk *c = TO_CHUNK(p);

	assert(IS_IN_USE(c) && "Double free?");
	CLEAR_IN_USE(c);

	struct chunk *left = c->left; /* This may be NULL if c is bottom. */
	if (left && !IS_IN_USE(left))
	{
		deallocate_chunk(c);
		c = left;
	}

	struct chunk *right = c->right;
	if (right != top && !IS_IN_USE(right))
		deallocate_chunk(right);
}

void *srealloc(void *p, size_t size)
{
	/* Handle unusual cases. */
	if (p == 0)
		return smalloc(size);
	if (size == 0)
	{
		sfree(p);
		return 0;
	}

	struct chunk *c = TO_CHUNK(p);
	size_t chunk_size = CHUNK_SIZE(c);
	size_t nb = TO_CHUNK_SIZE(size);
	
	if (chunk_size == nb)
		return p;
	if (nb < chunk_size)
		return allocate_chunk(c, nb);

	/* Check if we can expand right. */
	struct chunk *right = RIGHT(c);
	if (right != top && !IS_IN_USE(right)
	    && chunk_size + CHUNK_SIZE(right) >= nb)
	{
		deallocate_chunk(right);
		return allocate_chunk(c, nb);
	}

	/* No such luck. */
	void *q = smalloc(size);
	if (!q)
		return 0;
	memcpy(q, p, chunk_size - sizeof(struct chunk));
	sfree(p);
	return q;
}

/* Replace malloc and friends with our functions instead. */
static void *malloc_hook(size_t size, const void *caller)
{
	return smalloc(size);
}

static void *realloc_hook(void *ptr, size_t size, const void *caller)
{
	return srealloc(ptr, size);
}

static void free_hook(void *ptr, const void *caller)
{
	sfree(ptr);
}

static void *memalign_hook(size_t alignment, size_t size, const void *caller)
{
	abort();
}

static void init_hook(void)
{
	__malloc_hook = malloc_hook;
	__realloc_hook = realloc_hook;
	__free_hook = free_hook;
	__memalign_hook = memalign_hook;
}

void (*__MALLOC_HOOK_VOLATILE __malloc_initialize_hook)(void) = init_hook;
