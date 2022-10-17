DEF_CMD(PUSH, {
    int arg = 0;
    char arg_first_char = 0;
    sscanf(ARG_PTR, " %c", &arg_first_char);
    if (arg_first_char == '\'') {
        sscanf(ARG_PTR, " \'%c\'", (char*)&arg);
        log_printf(STATUS_REPORTS, "status", "Character %c (%02X) was detected.\n", (char)arg, (char)arg);
    } else {
        sscanf(ARG_PTR, "%d", &arg);
        log_printf(STATUS_REPORTS, "status", "Number %d was detected.\n", arg);
    }
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

DEF_CMD(DUP, {}, {
    _LOG_EMPT_STACK_("DUP");
    stack_push(STACK, stack_get(STACK));
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
        log_printf(ERROR_REPORTS, "error", "Invarid register index of %d in RGET, terminating.\n", reg_id);
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
        log_printf(ERROR_REPORTS, "error", "Invarid register index of %d in RGET, terminating.\n", reg_id);
        SHIFT = 0;
    }, ERRNO, EFAULT);
    _LOG_EMPT_STACK_("RSET");
    reg[reg_id] = (int)stack_get(STACK, ERRNO);
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
})

DEF_CMD(MGET, {}, {
    _LOG_EMPT_STACK_("MGET[key]");
    int key = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);
    _LOG_FAIL_CHECK_(0 <= key && key < (int)RAM_SIZE, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Incorrect memory index of %d was specified in MGET at %0*X.\n", key, sizeof(void*), EXEC_POINT);
        SHIFT = 0;
    }, ERRNO, EFAULT);
    stack_push(STACK, RAM[key], ERRNO);
})

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
})

DEF_CMD(VGET, {}, {
    _LOG_EMPT_STACK_("VGET[key]");
    int key = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);
    _LOG_FAIL_CHECK_(0 <= key && key < (int)VMD_SIZE, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Incorrect memory index of %d was specified in VGET at %0*X.\n", key, sizeof(void*), EXEC_POINT);
        SHIFT = 0;
    }, ERRNO, EFAULT);
    stack_push(STACK, VMD[key], ERRNO);
})