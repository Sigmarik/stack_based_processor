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