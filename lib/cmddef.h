/**
 * @file cmddef.h
 * @author Ilya Kudryashov (kudriashov.it@phystech.edu)
 * @brief List of commads.
 * @version 0.1
 * @date 2022-10-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifdef ASSEMBLER
    #ifndef ARG_PTR
        #error ARG_PTR was not defined while trying to access assembly code.
    #endif
    #ifndef GET_LABEL
        #error GET_LABEL was not defined while trying to access assembly code.
    #endif
    #ifndef CUR_ID
        #error CUR_ID was not defined while trying to access assembly code.
    #endif
    #ifndef BUF_WRITE
        #error BUF_WRITE was not defined while trying to access assembly code.
    #endif
    #ifndef ERRNO
        #error ERRNO was not defined while trying to access assembly code.
    #endif
#endif
#ifdef PROCESSOR
    #ifndef STACK
        #error STACK was not defined while trying to access execution code.
    #endif
    #ifndef ADDR_STACK
        #error ADDR_STACK was not defined while trying to access execution code.
    #endif
    #ifndef SHIFT
        #error SHIFT was not defined while trying to access execution code.
    #endif
    #ifndef EXEC_POINT
        #error EXEC_POINT was not defined while trying to access execution code.
    #endif
    #ifndef ARG_PTR
        #error ARG_PTR was not defined while trying to access execution code.
    #endif
    #ifndef ERRNO
        #error ERRNO was not defined while trying to access execution code.
    #endif
    #ifndef REG_SIZE
        #error REG_SIZE was not defined while trying to access execution code.
    #endif
#endif

DEF_CMD(END,   {}, {
    SHIFT = 0;
})

DEF_CMD(PUSH, {
    int arg = 0;
    sscanf(ARG_PTR, "%d", &arg);
    BUF_WRITE(&arg, sizeof(arg));
}, {
    int arg = 0;
    memcpy(&arg, ARG_PTR, sizeof(arg));
    stack_push(STACK, arg, ERRNO);
    SHIFT += (int)sizeof(arg);
})

DEF_CMD(POP, {}, {
    _LOG_EMPT_STACK_("POP");
    stack_pop(STACK, ERRNO);
})

DEF_CMD(OUT, {}, {
    _LOG_EMPT_STACK_("OUT");
    printf("%lld", stack_get(STACK, ERRNO));
})

DEF_CMD(ADD, {}, {
    _LOG_EMPT_STACK_("ADD[top]");
    int arg_a = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);
    _LOG_EMPT_STACK_("ADD[bottom]");
    int arg_b = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);
    stack_push(STACK, arg_a + arg_b);
})

DEF_CMD(SUB, {}, {
    _LOG_EMPT_STACK_("SUB[top]");
    int arg_a = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);
    _LOG_EMPT_STACK_("SUB[bottom]");
    int arg_b = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);
    stack_push(STACK, arg_a - arg_b);
})

DEF_CMD(MUL, {}, {
    _LOG_EMPT_STACK_("MUL[top]");
    int arg_a = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);
    _LOG_EMPT_STACK_("MUL[bottom]");
    int arg_b = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);
    stack_push(STACK, arg_a * arg_b);
})

DEF_CMD(DIV, {}, {
    _LOG_EMPT_STACK_("DIV[top]");
    int arg_a = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);
    _LOG_EMPT_STACK_("DIV[bottom]");
    int arg_b = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);
    stack_push(STACK, arg_a / arg_b);
})

DEF_CMD(DUP, {}, {
    _LOG_EMPT_STACK_("DUP");
    stack_push(STACK, stack_get(STACK));
})

DEF_CMD(ABORT, {}, {
    shift = 0;
    if (err_code) *err_code = EAGAIN;
})

DEF_CMD(JMP, {
    char lbl_name[LABEL_MAX_NAME_LENGTH] = "";
    sscanf(ARG_PTR, "%s", lbl_name);
    hash_t lbl_hash = get_hash(lbl_name, lbl_name + strlen(lbl_name));
    int argument = (int)get_label(lbl_hash, ERRNO) - (int)CUR_ID;
    BUF_WRITE(&argument, sizeof(argument));
}, {
    int dest = 0;
    memcpy(&dest, ARG_PTR, sizeof(dest));
    SHIFT = dest;
    _LOG_FAIL_CHECK_(SHIFT != 0, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "JMP argument was 0, terminating.\n");
    }, ERRNO, EFAULT);
})

DEF_CMD(JMPG, {
    char lbl_name[LABEL_MAX_NAME_LENGTH] = "";
    sscanf(ARG_PTR, "%s", lbl_name);
    hash_t lbl_hash = get_hash(lbl_name, lbl_name + strlen(lbl_name));
    int argument = (int)get_label(lbl_hash, ERRNO) - (int)CUR_ID;
    BUF_WRITE(&argument, sizeof(argument));
}, {
    int dest = 0;
    memcpy(&dest, ARG_PTR, sizeof(dest));
    _LOG_FAIL_CHECK_(dest != 0, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "JMP argument was 0, terminating.\n");
    }, ERRNO, EFAULT);
    _LOG_EMPT_STACK_("JMPG[top]");
    stack_content_t arg_a = stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);
    _LOG_EMPT_STACK_("JMPG[bottom]");
    stack_content_t arg_b = stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);
    if (arg_a > arg_b) SHIFT = dest;
    else SHIFT += (int)sizeof(dest);
})

DEF_CMD(RGET, {
    int arg = 0;
    sscanf(ARG_PTR, "%d", &arg);
    BUF_WRITE(&arg, sizeof(arg));
}, {
    int reg_id = 0;
    memcpy(&reg_id, ARG_PTR, sizeof(reg_id));
    SHIFT += (int)sizeof(reg_id);
    _LOG_FAIL_CHECK_(0 <= reg_id && reg_id <= (int)REG_SIZE, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Invarid register index of %d in RGET, terminating.\n", *(int*)(ptr + 1));
        SHIFT = 0;
    }, ERRNO, EFAULT);
    stack_push(STACK, reg[reg_id], ERRNO);
})

DEF_CMD(RSET, {
    int arg = 0;
    sscanf(ARG_PTR, "%d", &arg);
    BUF_WRITE(&arg, sizeof(arg));
}, {
    int reg_id = 0;
    memcpy(&reg_id, ARG_PTR, sizeof(reg_id));
    SHIFT += (int)sizeof(reg_id);
    _LOG_FAIL_CHECK_(0 <= reg_id && reg_id <= (int)REG_SIZE, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Invarid register index of %d in RGET, terminating.\n", *(int*)(ptr + 1));
        SHIFT = 0;
    }, ERRNO, EFAULT);
    _LOG_EMPT_STACK_("RSET");
    reg[reg_id] = (int)stack_get(STACK, ERRNO);
})

DEF_CMD(OUTC, {}, {
    _LOG_EMPT_STACK_("OUT");
    putc((int)stack_get(STACK, ERRNO), stdout);
})

DEF_CMD(CALL, {
    char lbl_name[LABEL_MAX_NAME_LENGTH] = "";
    sscanf(ARG_PTR, "%s", lbl_name);
    hash_t lbl_hash = get_hash(lbl_name, lbl_name + strlen(lbl_name));
    int argument = (int)get_label(lbl_hash, ERRNO) - (int)CUR_ID;
    BUF_WRITE(&argument, sizeof(argument));
}, {
    int dest = 0;
    memcpy(&dest, ARG_PTR, sizeof(dest));
    SHIFT += (int)sizeof(dest);
    stack_push(ADDR_STACK, (stack_content_t)EXEC_POINT + SHIFT, ERRNO);
    SHIFT = dest;
    _LOG_FAIL_CHECK_(SHIFT != 0, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "CALL argument was 0, terminating.\n");
    }, ERRNO, EFAULT);
})

DEF_CMD(RET, {}, {
    _LOG_FAIL_CHECK_(ADDR_STACK->size, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Address stack was zero when RET was called, terminating.");
        shift = 0;
        break;
    }, ERRNO, EFAULT);
    uintptr_t dest = (uintptr_t) stack_get(ADDR_STACK, ERRNO);
    stack_pop(ADDR_STACK, ERRNO);
    SHIFT = (int)(dest - (uintptr_t)EXEC_POINT);
})