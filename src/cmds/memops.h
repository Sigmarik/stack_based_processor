#define DISASM_COMPLEX do{ \
    switch (usage) { \
        case 0: { \
            fprintf(OUT_FILE, "%d", arg); \
            if (isprint((char)arg)) { \
                fprintf(OUT_FILE, " # \'%c\'", (char)arg); \
            } \
        } break; \
 \
        case USE_MEMORY: { \
            fprintf(OUT_FILE, "[%d]", arg); \
        } break; \
 \
        case USE_REGISTER: { \
            fprintf(OUT_FILE, "R%cX", (char)(arg + 'A')); \
        } break; \
 \
        case USE_REGISTER | USE_MEMORY: { \
            int reg_id = arg & 0xFF; \
 \
            int pointer_shift = 0; \
            memcpy(&pointer_shift, (char*)&arg + 1, sizeof(pointer_shift) - 1); \
            if (arg < 0) *((char*)&pointer_shift + sizeof(pointer_shift) - 1) = (char)0xFF; \
 \
            fprintf(OUT_FILE, "[R%cX + %d]", (char)(reg_id + 'A'), pointer_shift); \
        } break; \
 \
        default: {log_printf(ERROR_REPORTS, "error", "Usage tag had unexpected value while disassembling %0*X.\n", sizeof(void*), EXEC_POINT);} \
    } \
} while (0)

DEF_CMD(PUSH, {
    PPArgument arg = read_pparg(ARG_PTR);
    BUF_WRITE(&arg.value, sizeof(arg.value));
    BUF_PTR[0] |= arg.props;
    log_printf(STATUS_REPORTS, "status", "Parser decided on the value = %d, properties = %d.\n", arg.value, arg.props);
}, {
    int arg = 0;
    memcpy(&arg, ARG_PTR, sizeof(arg));
    char usage = *EXEC_POINT & 3;
                            /* ^ first two bits */
    log_printf(STATUS_REPORTS, "status", "Argument = %d, command mask = %d.\n", arg, usage);
    int status = 0;
    int* subject = link_argument(usage, &arg, RAM, REG, &status);
    if (status) {
        SHIFT = 0;
        log_printf(ERROR_REPORTS, "error", "Failed to link command argument at %0*X.\n", sizeof(void*), EXEC_POINT);
    } else {
        stack_push(STACK, *subject, ERRNO);
        SHIFT += (int)sizeof(arg);
    }
}, {
    int arg = 0;
    memcpy(&arg, ARG_PTR, sizeof(arg));
    char usage = *EXEC_POINT & 3;
                            /* ^ first two bits */
    log_printf(STATUS_REPORTS, "status", "Argument = %d, command mask = %d.\n", arg, usage);
    DISASM_COMPLEX;
    SHIFT += (int)sizeof(arg);
})

DEF_CMD(POP, {}, {
    _LOG_EMPT_STACK_("POP");
    stack_pop(STACK, ERRNO);
}, {})

DEF_CMD(MOVE, {
    PPArgument arg = read_pparg(ARG_PTR);
    BUF_WRITE(&arg.value, sizeof(arg.value));
    BUF_PTR[0] |= arg.props;
    log_printf(STATUS_REPORTS, "status", "Parser decided on value = %d, properties = %d.\n", arg.value, arg.props);
}, {
    _LOG_EMPT_STACK_("MOVE");
    int arg = 0;
    memcpy(&arg, ARG_PTR, sizeof(arg));
    char usage = *EXEC_POINT & 3;
                            /* ^ first two bits */
    log_printf(STATUS_REPORTS, "status", "Argument = %d, command mask = %d.\n", arg, usage);
    int status = 0;
    int* subject = link_argument(usage, &arg, RAM, REG, &status);
    if (status) {
        SHIFT = 0;
        log_printf(ERROR_REPORTS, "error", "Failed to link command argument at %0*X.\n", sizeof(void*), EXEC_POINT);
    } else {
        *subject = (int)stack_get(STACK, ERRNO);
        stack_pop(STACK, ERRNO);
        SHIFT += (int)sizeof(arg);
    }
}, {
    int arg = 0;
    memcpy(&arg, ARG_PTR, sizeof(arg));
    char usage = *EXEC_POINT & 3;
                            /* ^ first two bits */
    log_printf(STATUS_REPORTS, "status", "Argument = %d, command mask = %d.\n", arg, usage);
    DISASM_COMPLEX;
    SHIFT += (int)sizeof(arg);
})

DEF_CMD(DUP, {}, {
    _LOG_EMPT_STACK_("DUP");
    stack_push(STACK, stack_get(STACK));
}, {})

DEF_CMD(VSET, {}, {
    _LOG_EMPT_STACK_("VSET[key]");
    int key = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);
    _LOG_EMPT_STACK_("VSET[value]");
    int value = (int)stack_get(STACK, ERRNO);
    _LOG_FAIL_CHECK_(0 <= key && key < (int)VMD_SIZE, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Incorrect memory index of %d was specified in MSET at %0*X.\n", key, sizeof(void*), EXEC_POINT);
        SHIFT = 0;
    }, ERRNO, EFAULT);
    VMD.content[key] = value;
}, {})

DEF_CMD(VGET, {}, {
    _LOG_EMPT_STACK_("VGET[key]");
    int key = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);
    _LOG_FAIL_CHECK_(0 <= key && key < (int)VMD_SIZE, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Incorrect memory index of %d was specified in VGET at %0*X.\n", key, sizeof(void*), EXEC_POINT);
        SHIFT = 0;
    }, ERRNO, EFAULT);
    stack_push(STACK, VMD.content[key], ERRNO);
}, {})

#undef DISASM_COMPLEX