#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define STACK_SIZE 2048
#define ALIGNMENT 8 // Alignment to 8byte boundary

typedef struct {
    uint8_t* base;
    uint8_t* top;
    uint8_t* limit;
    size_t allocations; // block count
} StackAllocator;

uint8_t stackPool[STACK_SIZE];

void* alignPointer(void* pointer, size_t alignment) {
    return (void*)(((uintptr_t)pointer + (alignment - 1)) & ~(alignment - 1));
}

void stackInit(StackAllocator* stack) {
    stack->base = stackPool;
    stack->top = alignPointer(stackPool, ALIGNMENT);
    stack->limit = stackPool + STACK_SIZE;
    stack->allocations = 0;
}

void* stackAlloc(StackAllocator* stack, size_t size) {
    uint8_t* alignedTop = (uint8_t*)alignPointer(stack->top, ALIGNMENT);
    if (alignedTop + size > stack->limit) return NULL;
    void* block = alignedTop;
    stack->top = alignedTop + size;
    stack->allocations++;
    return block;
}

void stackReset(StackAllocator* stack) {
    stack->top = alignPointer(stack->base, ALIGNMENT);
    stack->allocations = 0;
}

// Functions that intentionally generate memory leaks
void causeMemoryLeak(StackAllocator* stack) {
    // Allocates memory but does not reset or release it
    stackAlloc(stack, 100); // Allocate 100 bytes
}

int main() {
    StackAllocator stack;
    stackInit(&stack);

    // Intentionally generate memory leaks
    causeMemoryLeak(&stack);

    // Reset stack allocator and check for memory leaks
    // Directly check for memory leaks
    if (stack.allocations != 0) {
        printf("Memory leak detected: %zu allocations not freed.\n", stack.allocations);
    } else {
        printf("No memory leak detected.\n");
    }

    return 0;
}
