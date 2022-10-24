#include "common.h"

#include <stdlib.h>
#include <stdarg.h>

void** bundle(size_t count, ...) {
    va_list args;
    va_start(args, count);

    void** array = (void**) calloc(count, sizeof(*array));
    track_allocation(array, free);

    for (size_t index = 0; index < count; index++) {
        array[index] = va_arg(args, void*);
    }

    va_end(args);
    return array;
}

void MemorySegment_ctor(MemorySegment* segment) {
    segment->content = (int*) calloc(segment->size, sizeof(*segment->content));
}

void MemorySegment_dtor(MemorySegment* segment) {
    free(segment->content);
    segment->content = NULL;
    segment->size = 0;
}

void _MemorySegment_dump(MemorySegment* segment, unsigned int importance) {
    for (size_t id = 0; id < segment->size; ++id) {
        _log_printf(importance, "dump", "[%6ld] = %d\n", id, segment->content[id]);
    }
}

void FrameBuffer_ctor(FrameBuffer* buffer) {
    buffer->content = (int*) calloc(buffer->width * buffer->height, sizeof(*buffer->content));
}

void FrameBuffer_dtor(FrameBuffer* buffer) {
    free(buffer->content);
    buffer->content = NULL;
    buffer->width = 0;
    buffer->height = 0;
}

int clamp(const int value, const int left, const int right) {
    if (value < left) return left;
    if (value > right) return right;
    return value;
}