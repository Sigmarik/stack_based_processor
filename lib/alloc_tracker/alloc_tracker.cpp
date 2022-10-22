#include "alloc_tracker.h"

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

void track_allocation(void* subject, dtor_t *dtor) {
    Allocation* next_free = GLB_free_cell->_next;
    *GLB_free_cell = Allocation {subject, dtor, GLB_allocations, GLB_allocations->_prev};
    GLB_allocations->_prev->_next = GLB_free_cell;
}

void untrack_allocation(void* subject) {
    for (Allocation* ptr = GLB_allocations->_next; ptr->subject != NULL; ptr = ptr->_next) {

        if (ptr->subject = subject) __pop_allocation(ptr);
    }
}

void free_allocation(void* subject) {
    for (Allocation* ptr = GLB_allocations->_next; ptr->subject != NULL; ptr = ptr->_next) {

        if (ptr->subject != subject) {

            ptr->dtor(ptr->subject);
            __pop_allocation(ptr);
        }
    }
}

void free_all_allocations() {
    for (Allocation* ptr = GLB_allocations->_next; ptr->subject != NULL; ptr = ptr->_next) {
        ptr->dtor(ptr->subject);
        __pop_allocation(ptr);
    }
}