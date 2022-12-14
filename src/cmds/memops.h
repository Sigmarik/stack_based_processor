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
        PUSH(*subject);
        SHIFT += (int)sizeof(arg);
    }
}, {
    int arg = 0;
    memcpy(&arg, ARG_PTR, sizeof(arg));
    char usage = *EXEC_POINT & 3;
                            /* ^ first two bits */
    log_printf(STATUS_REPORTS, "status", "Argument = %d, command mask = %d.\n", arg, usage);
    write_argument(OUT_FILE, usage, arg);
    SHIFT += (int)sizeof(arg);
})

DEF_CMD(POP, {}, {
    POP_TOP();
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
        GET_TOP(stack_content_t value); POP_TOP();
        *subject = (int)value;
        SHIFT += (int)sizeof(arg);
    }
}, {
    int arg = 0;
    memcpy(&arg, ARG_PTR, sizeof(arg));
    char usage = *EXEC_POINT & 3;
                            /* ^ first two bits */
    log_printf(STATUS_REPORTS, "status", "Argument = %d, command mask = %d.\n", arg, usage);
    write_argument(OUT_FILE, usage, arg);
    SHIFT += (int)sizeof(arg);
})

DEF_CMD(DUP, {}, {
    GET_TOP(stack_content_t value);
    PUSH((int)value);
}, {})

DEF_CMD(VSET, {}, {
    GET_TOP(stack_content_t key); POP_TOP();
    GET_TOP(stack_content_t value);
    _LOG_FAIL_CHECK_(0 <= key && key < (stack_content_t)VMD_SIZE, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Incorrect memory index of %d was specified in MSET at %0*X.\n", key, sizeof(void*), EXEC_POINT);
        SHIFT = 0;
    }, ERRNO, EFAULT);
    VMD.content[key] = (int)value;
}, {})

DEF_CMD(VGET, {}, {
    GET_TOP(stack_content_t key); POP_TOP();
    _LOG_FAIL_CHECK_(0 <= key && key < (stack_content_t)VMD_SIZE, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Incorrect memory index of %d was specified in VGET at %0*X.\n", key, sizeof(void*), EXEC_POINT);
        SHIFT = 0;
    }, ERRNO, EFAULT);
    stack_push(STACK, VMD.content[key], ERRNO);
}, {})