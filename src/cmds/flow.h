#include "cond_jumps.h"

DEF_CMD(END, {}, {
    SHIFT = 0;
}, {})

DEF_CMD(ABORT, {}, {
    shift = 0;
    if (err_code) *err_code = EAGAIN; // TODO: code repetition
}, {})

DEF_CMD(JMP, {
    char lbl_name[LABEL_MAX_NAME_LENGTH] = "";
    int argument = 0;

    sscanf(ARG_PTR, "%d", &argument);

    if (argument == 0) {
        sscanf(ARG_PTR, "%s", lbl_name);
        hash_t lbl_hash = get_simple_hash(lbl_name, lbl_name + strlen(lbl_name));
        argument = (int)get_label(LABEL_LIST, lbl_hash, ERRNO) - (int)CUR_ID;
        // TODO: code repetition...
    }

    BUF_WRITE(&argument, sizeof(argument));
}, {
    int dest = 0;
    memcpy(&dest, ARG_PTR, sizeof(dest));
    SHIFT = dest;
    _LOG_FAIL_CHECK_(SHIFT != 0, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "JMP argument was 0, terminating.\n");
    }, ERRNO, EFAULT);
}, {
    int dest = 0;
    memcpy(&dest, ARG_PTR, sizeof(dest));
    SHIFT += (int)sizeof(dest); // TODO: extreme repetition...
    fprintf(OUT_FILE, "%d", dest); 
})

DEF_CMD(CALL, {
    char lbl_name[LABEL_MAX_NAME_LENGTH] = ""; // TODO: look same thing 3rd time, coool! (not really)
    int argument = 0;                          //       You see, coming up with a way to do it without repetition
                                               //       is the real challange that should have been addressed here!

    sscanf(ARG_PTR, "%d", &argument);

    if (argument == 0) {
        sscanf(ARG_PTR, "%s", lbl_name);
        hash_t lbl_hash = get_simple_hash(lbl_name, lbl_name + strlen(lbl_name));
        argument = (int)get_label(LABEL_LIST, lbl_hash, ERRNO) - (int)CUR_ID;
    }

    BUF_WRITE(&argument, sizeof(argument));
}, {
    int dest = 0;
    memcpy(&dest, ARG_PTR, sizeof(dest)); // TODO: I'm tired, it's all the same!!

    SHIFT += (int)sizeof(dest);

    stack_push(ADDR_STACK, (stack_content_t)EXEC_POINT + SHIFT, ERRNO);

    SHIFT = dest;

    _LOG_FAIL_CHECK_(SHIFT != 0, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "CALL argument was 0, terminating.\n");
    }, ERRNO, EFAULT);
}, {
    int dest = 0;
    memcpy(&dest, ARG_PTR, sizeof(dest));

    SHIFT += (int)sizeof(dest);

    fprintf(OUT_FILE, "%d", dest);
})

DEF_CMD(RET, {}, {
    _LOG_FAIL_CHECK_(ADDR_STACK->size, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Address stack was empty when RET was called, terminating.");
        shift = 0;
        break;
    }, ERRNO, EFAULT);

    uintptr_t dest = (uintptr_t) stack_get(ADDR_STACK, ERRNO);
    stack_pop(ADDR_STACK, ERRNO);

    SHIFT = (int)(dest - (uintptr_t)EXEC_POINT);
}, {})
