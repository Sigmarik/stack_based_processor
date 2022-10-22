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
    log_printf(STATUS_REPORTS, "status", "Parser decided on value = %d, properties = %d.\n", arg.value, arg.props);
}, {
    int arg = 0;
    memcpy(&arg, ARG_PTR, sizeof(arg));
    char usage = *EXEC_POINT & 3;
                            /* ^ first two bits */
    log_printf(STATUS_REPORTS, "status", "Argument = %d, command mask = %d.\n", arg, usage);
    switch (usage) {
        case 0: {
            stack_push(STACK, arg, ERRNO);
        } break;

        case USE_MEMORY: {
            _LOG_FAIL_CHECK_(0 <= arg && arg < (int)RAM_SIZE, "error", ERROR_REPORTS, {
                log_printf(ERROR_REPORTS, "error", "Incorrect memory index of %d was specified in PUSH at %0*X.\n", arg, sizeof(void*), EXEC_POINT);
                SHIFT = 0;
            }, ERRNO, EFAULT);

            stack_push(STACK, RAM[arg], ERRNO);
        } break;

        case USE_REGISTER: {
            _LOG_FAIL_CHECK_(0 <= arg && arg < (int)REG_SIZE, "error", ERROR_REPORTS, {
                log_printf(ERROR_REPORTS, "error", "Incorrect register index of %d was specified in PUSH at %0*X.\n", arg, sizeof(void*), EXEC_POINT);
                SHIFT = 0;
            }, ERRNO, EFAULT);

            stack_push(STACK, reg[arg], ERRNO);
        } break;

        case USE_REGISTER | USE_MEMORY: {
            int reg_id = arg & 0xFF;

            int pointer_shift = 0;
            memcpy(&pointer_shift, (char*)&arg + 1, sizeof(pointer_shift) - 1);
            if (arg < 0) *((char*)&pointer_shift + sizeof(pointer_shift) - 1) = (char)0xFF;

            _LOG_FAIL_CHECK_(0 <= reg_id && reg_id < (int)REG_SIZE, "error", ERROR_REPORTS, {
                log_printf(ERROR_REPORTS, "error", "Incorrect register index of %d was detected while executing PUSH at %0*X.\n", reg_id, sizeof(void*), EXEC_POINT);
                SHIFT = 0;
            }, ERRNO, EFAULT);

            int ram_index = reg[reg_id] + pointer_shift;

            _LOG_FAIL_CHECK_(0 <= ram_index && ram_index < (int)RAM_SIZE, "error", ERROR_REPORTS, {
                log_printf(ERROR_REPORTS, "error", "Incorrect register index of %d was detected while executing PUSH at %0*X.\n", ram_index, sizeof(void*), EXEC_POINT);
                SHIFT = 0;
            }, ERRNO, EFAULT);

            stack_push(STACK, RAM[ram_index], ERRNO);
        } break;

        default: {log_printf(ERROR_REPORTS, "error", "Usage tag had unexpected value while executing %0*X.\n", sizeof(void*), EXEC_POINT);}
    }
    SHIFT += (int)sizeof(arg);
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

//     call Start + rax

// Start:
//     call ABSOLUTE_IMPORTANCE
//     ret

//     call BIG_ENDIAN
//     ret

//     call CMD_ABORT
//     ret
    
//     call DATA_UPDATES
//     ret

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
    switch (usage) {
        case 0: {
            stack_pop(STACK, ERRNO);
        } break;

        case USE_MEMORY: {
            _LOG_FAIL_CHECK_(0 <= arg && arg < (int)RAM_SIZE, "error", ERROR_REPORTS, {
                log_printf(ERROR_REPORTS, "error", "Incorrect memory index of %d was specified in MOVE at %0*X.\n", arg, sizeof(void*), EXEC_POINT);
                SHIFT = 0;
            }, ERRNO, EFAULT);

            RAM[arg] = (int)stack_get(STACK, ERRNO);
            stack_pop(STACK);
        } break;

        case USE_REGISTER: {
            _LOG_FAIL_CHECK_(0 <= arg && arg < (int)REG_SIZE, "error", ERROR_REPORTS, {
                log_printf(ERROR_REPORTS, "error", "Incorrect register index of %d was specified in MOVE at %0*X.\n", arg, sizeof(void*), EXEC_POINT);
                SHIFT = 0;
            }, ERRNO, EFAULT);

            reg[arg] = (int)stack_get(STACK, ERRNO);
            stack_pop(STACK, ERRNO);
        } break;

        case USE_REGISTER | USE_MEMORY: {
            int reg_id = arg & 0xFF;

            int pointer_shift = 0;
            memcpy(&pointer_shift, (char*)&arg + 1, sizeof(pointer_shift) - 1);

            _LOG_FAIL_CHECK_(0 <= reg_id && reg_id < (int)REG_SIZE, "error", ERROR_REPORTS, {
                log_printf(ERROR_REPORTS, "error", "Incorrect register index of %d was detected while executing MOVE at %0*X.\n", reg_id, sizeof(void*), EXEC_POINT);
                SHIFT = 0;
            }, ERRNO, EFAULT);

            int ram_index = reg[reg_id] + pointer_shift;

            _LOG_FAIL_CHECK_(0 <= ram_index && ram_index < (int)RAM_SIZE, "error", ERROR_REPORTS, {
                log_printf(ERROR_REPORTS, "error", "Incorrect register index of %d was detected while executing MOVE at %0*X.\n", ram_index, sizeof(void*), EXEC_POINT);
                SHIFT = 0;
            }, ERRNO, EFAULT);

            RAM[ram_index] = (int)stack_get(STACK, ERRNO);
            stack_pop(STACK, ERRNO);
        } break;

        default: {log_printf(ERROR_REPORTS, "error", "Usage tag had unexpected value while executing %0*X.\n", sizeof(void*), EXEC_POINT);}
    }
    SHIFT += (int)sizeof(arg);
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

DEF_CMD(RGET, {
    int arg = 0;
    sscanf(ARG_PTR, "%d", &arg);
    BUF_WRITE(&arg, sizeof(arg));
}, {
    int reg_id = 0;
    memcpy(&reg_id, ARG_PTR, sizeof(reg_id));
    SHIFT += (int)sizeof(reg_id);
    _LOG_FAIL_CHECK_(0 <= reg_id && reg_id <= (int)REG_SIZE, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Invarid register index of %d in RGET, terminating.\n", reg_id);
        SHIFT = 0;
    }, ERRNO, EFAULT);
    stack_push(STACK, reg[reg_id], ERRNO);
}, {
    int reg_id = 0;
    memcpy(&reg_id, ARG_PTR, sizeof(reg_id));
    SHIFT += (int)sizeof(reg_id);
    fprintf(OUT_FILE, "R%cX", (char)(reg_id + 'A'));
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
        log_printf(ERROR_REPORTS, "error", "Invarid register index of %d in RGET, terminating.\n", reg_id);
        SHIFT = 0;
    }, ERRNO, EFAULT);
    _LOG_EMPT_STACK_("RSET");
    reg[reg_id] = (int)stack_get(STACK, ERRNO);
}, {
    int reg_id = 0;
    memcpy(&reg_id, ARG_PTR, sizeof(reg_id));
    SHIFT += (int)sizeof(reg_id);
    fprintf(OUT_FILE, "R%cX", (char)(reg_id + 'A'));
})

DEF_CMD(MSET, {}, {
    _LOG_EMPT_STACK_("MSET[key]");
    int key = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);
    _LOG_EMPT_STACK_("MSET[value]");
    int value = (int)stack_get(STACK, ERRNO);
    _LOG_FAIL_CHECK_(0 <= key && key < (int)RAM_SIZE, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Incorrect memory index of %d was specified in MSET at %0*X.\n", key, sizeof(void*), EXEC_POINT);
        SHIFT = 0;
    }, ERRNO, EFAULT);
    RAM[key] = value;
}, {})

DEF_CMD(MGET, {}, {
    _LOG_EMPT_STACK_("MGET[key]");
    int key = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);
    _LOG_FAIL_CHECK_(0 <= key && key < (int)RAM_SIZE, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Incorrect memory index of %d was specified in MGET at %0*X.\n", key, sizeof(void*), EXEC_POINT);
        SHIFT = 0;
    }, ERRNO, EFAULT);
    stack_push(STACK, RAM[key], ERRNO);
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
    VMD[key] = value;
}, {})

DEF_CMD(VGET, {}, {
    _LOG_EMPT_STACK_("VGET[key]");
    int key = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);
    _LOG_FAIL_CHECK_(0 <= key && key < (int)VMD_SIZE, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Incorrect memory index of %d was specified in VGET at %0*X.\n", key, sizeof(void*), EXEC_POINT);
        SHIFT = 0;
    }, ERRNO, EFAULT);
    stack_push(STACK, VMD[key], ERRNO);
}, {})

#undef DISASM_COMPLEX