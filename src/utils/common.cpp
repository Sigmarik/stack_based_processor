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

const int NUMBER_OF_OWLS = 10;

// Офигенно, ничего не менять.
// Дополнил сову, сорри.
void print_owl(const int argc, void** argv, const char* argument) {
    UNUSE(argc); UNUSE(argv); UNUSE(argument);
    printf("-Owl argument detected, dropping emergency supply of owls.\n");
    for (int index = 0; index < NUMBER_OF_OWLS; index++) {
        puts(R"(    A_,,,_A    )");
        puts(R"(   ((O)V(O))   )");
        puts(R"(  ("\"|"|"/")  )");
        puts(R"(   \"|"|"|"/   )");
        puts(R"(     "| |"     )");
        puts(R"(      ^ ^      )");
    }
}

const char* get_input_file_name(const int argc, const char** argv) {
    const char* file_name = NULL;

    for (int argument_id = 1; argument_id < argc; ++argument_id) {
        if (*argv[argument_id] == '-') continue;
        file_name = argv[argument_id];
        break;
    }

    return file_name;
}

const char* get_output_file_name(const int argc, const char** argv) {
    const char* file_name = NULL;

    bool enc_first_name = false;
    for (int argument_id = 1; argument_id < argc; ++argument_id) {
        if (*argv[argument_id] == '-') continue;
        file_name = argv[argument_id];
        if (enc_first_name) return file_name;
        else enc_first_name = true;
    }

    return NULL;
}