
#include <stdint.h>
#include <stdbool.h>

#include "mm.h"

#ifdef MM_DEBUG
#include "print.h"
#endif

#define PAGE_SIZE (64 * 1024)

struct block_info {
	struct block_info *previous;
	struct block_info *next;
	bool free;
	size_t size;
};
#define BLOCK_INFO_SIZE (sizeof(struct block_info))

static size_t current_pages;
static uintptr_t heap_top;
static struct block_info *first_block;
static struct block_info *last_block;
static struct block_info *first_free_block;

size_t grow_memory(size_t pages) {
    size_t current_pages = __builtin_wasm_current_memory();
    __builtin_wasm_grow_memory(pages);
    return current_pages;
}

uintptr_t grow_heap(size_t inc) {
	uintptr_t old_heap_top = heap_top;

	heap_top += inc;

	uintptr_t heap_max = current_pages * PAGE_SIZE - 1;
	if(heap_top > heap_max) {
		size_t diff = heap_top - heap_max;
		size_t pages = (diff + (PAGE_SIZE - 1)) / PAGE_SIZE;
#ifdef MM_DEBUG
		prints("Heap too small by ");
		printi(diff);
		prints(" bytes, ");
		printi(pages);
		prints(" pages");
		printc('\n');
#endif
		current_pages = grow_memory(pages) + pages;
	}

#ifdef MM_DEBUG
	prints("Heap now ends at ");
	printptr((void *) heap_top);
	printc('\n');
#endif

	return old_heap_top;
}

__attribute__((visibility("default")))
void mm_init() {
	current_pages = grow_memory(0);
	heap_top = current_pages * PAGE_SIZE;
	first_block = NULL;
	last_block = NULL;
	first_free_block = NULL;

#ifdef MM_DEBUG
	prints("BLOCK_INFO_SIZE: ");
	printi(BLOCK_INFO_SIZE);
	printc('\n');
	prints("Heap start: ");
	printptr((void *) heap_top);
	printc('\n');
#endif
}

void *malloc(size_t size) {
	// TODO: alignment
	struct block_info *block = first_free_block;
	while(block != NULL) {
		if(block->free) {
			if(block->size >= size) {
#ifdef MM_DEBUG
				prints("Found free block with sufficient size\n");
#endif
				if(block->size - size > BLOCK_INFO_SIZE) {
					uintptr_t next_block_addr = (uintptr_t) block + BLOCK_INFO_SIZE + size;

					struct block_info *next_block = (struct block_info *) next_block_addr;
					next_block->previous = block;
					next_block->next = block->next;
					next_block->free = true;
					next_block->size = block->size - size - BLOCK_INFO_SIZE;

					block->next = next_block;
					block->size = size;

					first_free_block = next_block;
				} else {
					first_free_block = block->next;
					while(first_free_block != NULL && !first_free_block->free) {
						first_free_block = first_free_block->next;
					}
				}
				block->free = false;
				return (void *) ((uintptr_t) block + BLOCK_INFO_SIZE);
			} else if((uintptr_t) block == (uintptr_t) last_block) {
				grow_heap(size - block->size);
				block->size = size;
				block->free = false;
				return (void *) ((uintptr_t) block + BLOCK_INFO_SIZE);
			}
		}
		block = block->next;
	}

#ifdef MM_DEBUG
	prints("No free block with sufficient size found\n");
#endif

	struct block_info *new_block = (struct block_info *) grow_heap(BLOCK_INFO_SIZE + size);
	new_block->previous = last_block;
	new_block->next = NULL;
	new_block->free = false;
	new_block->size = size;

	if(first_block == NULL) {
		first_block = new_block;
	}

	if(last_block != NULL) {
		last_block->next = new_block;
	}
	last_block = new_block;

	return (void *) ((uintptr_t) new_block + BLOCK_INFO_SIZE);
}

void free(void *ptr) {
	struct block_info *block = (struct block_info *) ((uintptr_t) ptr - BLOCK_INFO_SIZE);
	
	block->free = true;
	
	// Merge consecutive free blocks
	if(block->previous && block->previous->free) {
		block->previous->size += BLOCK_INFO_SIZE + block->size;
		block->previous->next = block->next;
		block->next->previous = block->previous;
		if((uintptr_t) block == (uintptr_t) last_block) {
			last_block = block->previous;
		}
		block = block->previous;
	}
	if(block->next && block->next->free) {
		block->size += BLOCK_INFO_SIZE + block->next->size;
		if((uintptr_t) block->next == (uintptr_t) last_block) {
			last_block = block;
		}
		block->next = block->next->next;
		block->next->previous = block;
	}

	if(first_free_block == NULL || (uintptr_t) block < (uintptr_t) first_free_block) {
		first_free_block = block;
	}

	// TODO: if this is the last block, release it

#ifdef MM_DEBUG
	prints("Freed block at ");
	printptr(ptr);
	printc('\n');
#endif
}

void *calloc(size_t nmemb, size_t size) {
	size_t full_size = nmemb * size;
	if(nmemb != 0 && full_size / nmemb != size) {
#ifdef MM_DEBUG
		prints("calloc() multiplication overflow: ");
		printi(nmemb);
		prints(" * ");
		printi(size);
		prints(" > SIZE_MAX\n");
#endif
		return NULL;
	}

	void *ptr = malloc(full_size);
	if(ptr) {
		//memset(ptr, 0, full_size);
	}

	return ptr;
}

void *realloc(void *ptr, size_t size) {
	// TODO
	return NULL;
}

#ifdef MM_DEBUG
void print_heap() {
	struct block_info *block = first_block;
	if(block == NULL) {
		prints("No blocks\n");
		return;
	}

	prints("First block: ");
	printptr(first_block);
	printc('\n');
	prints("Last block: ");
	printptr(last_block);
	printc('\n');
	prints("First free block: ");
	printptr(first_free_block);
	printc('\n');

	if(block->previous) {
		prints("FIRST BLOCK HAS A PREVIOUS BLOCK\n");
	}

	uintptr_t next_expected_addr = (uintptr_t) block;
	
	struct block_info *prev_block = NULL;
	while(block != NULL) {
		prints("block at ");
		printptr(block);
		prints(": ");
		printi(block->size);
		prints(" bytes, ");
		prints(block->free ? "free" : "used");
		printc('\n');

		if((uintptr_t) block != next_expected_addr) {
			prints("BLOCKS ARE NOT CONSECUTIVE: last block ends at ");
			printptr((void *) next_expected_addr);
			prints(" but block starts at ");
			printptr(block);
			printc('\n');
		}

		if(block->previous != prev_block) {
			prints("WRONG POINTER TO PREVIOUS BLOCK: block->previous is ");
			printptr(block->previous);
			prints(" but previous block is at ");
			printptr(prev_block);
			printc('\n');
		}

		if(block->next && (uintptr_t) block >= (uintptr_t) block->next) {
			prints("NEXT BLOCK IS NOT LOCATED AFTER CURRENT BLOCK: block is at ");
			printptr(block);
			prints(" but block->next points to ");
			printptr(block->next);
		}

		next_expected_addr = (uintptr_t) block + BLOCK_INFO_SIZE + block->size;
		prev_block = block;
		block = block->next;
	}
}
#endif
