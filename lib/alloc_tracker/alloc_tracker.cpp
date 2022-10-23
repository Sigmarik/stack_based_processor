#include "alloc_tracker.h"

#include <errno.h>

static struct Allocation {
    void* subject = NULL;
    dtor_t* dtor = NULL;
    Allocation* _next = NULL;
    Allocation* _prev = NULL;
} GLB_allocations[MAX_ALLOCATIONS] = {};

static Allocation* GLB_free_cell = GLB_allocations + 1;

/**
 * @brief Fill allocation buffer with default values and link them.
 * 
 * @return 
 */
static char __fill_allocations() {
    for (Allocation* ptr = GLB_allocations; ptr < GLB_allocations + MAX_ALLOCATIONS; ptr++) {
        *ptr = Allocation {};
        ptr->_next = ptr + 1;
        ptr->_prev = ptr - 1;
    }
    GLB_allocations[MAX_ALLOCATIONS - 1]._next = NULL;
    GLB_allocations[0]._prev = GLB_allocations;
    GLB_allocations[0]._prev = GLB_allocations;
    GLB_free_cell = GLB_allocations + 1;

    return '\0';
}

static char __allocations_filler = __fill_allocations();

/**
 * @brief Pop element from the list by its address.
 * 
 * @param ptr pointer to the element
 */
static void __pop_allocation(Allocation* ptr) {
    ptr->subject = NULL;
    ptr->dtor = NULL;

    ptr->_next->_prev = ptr->_prev;
    ptr->_prev->_next = ptr->_next;

    ptr->_next = GLB_free_cell;
    GLB_free_cell = ptr;
}

void _track_allocation(void* subject, dtor_t *dtor) {
    Allocation* next_free = GLB_free_cell->_next;
    *GLB_free_cell = Allocation {subject, dtor, GLB_allocations, GLB_allocations->_prev};
    GLB_allocations->_prev->_next = GLB_free_cell;
    GLB_allocations->_prev = GLB_free_cell;
    log_printf(STATUS_REPORTS, "status", "Started tracking address %p at index %ld.\n", subject, GLB_free_cell - GLB_allocations);
    GLB_free_cell = next_free;
}

void untrack_allocation(void* subject) {
    for (Allocation* ptr = GLB_allocations->_next; ptr != GLB_allocations; ptr = ptr->_next) {

        if (ptr->subject == subject) { __pop_allocation(ptr); break; }
    }
}

void free_allocation(void* subject) {
    for (Allocation* ptr = GLB_allocations->_next; ptr != GLB_allocations; ptr = ptr->_next) {

        if (ptr->subject != subject) {

            ptr->dtor(ptr->subject);
            __pop_allocation(ptr);

            break;
        }
    }
}

void free_all_allocations() {
    for (Allocation* ptr = GLB_allocations->_next; ptr != GLB_allocations; ptr = ptr->_next) {
        log_printf(STATUS_REPORTS, "status", "Processing address %p from cell %ld.\n", ptr->subject, ptr - GLB_allocations);
        ptr->dtor(ptr->subject);
    }
    __fill_allocations();
}

void free_var(void** ptr) {
    _LOG_FAIL_CHECK_(ptr, "error", ERROR_REPORTS, return, &errno, EFAULT);
    log_printf(STATUS_REPORTS, "status", "Freeing address %p.\n", *ptr);
    if (*ptr) free(*ptr);
    *ptr = NULL;
}

void void_free(void* ptr) {
    if (ptr) free(ptr);
}