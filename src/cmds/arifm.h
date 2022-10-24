DEF_CMD(ADD, {}, {
    _LOG_EMPT_STACK_("ADD[top]");
    stack_content_t arg_a = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);

    _LOG_EMPT_STACK_("ADD[bottom]");
    stack_content_t arg_b = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);

    stack_push(STACK, arg_a + arg_b);
}, {})

DEF_CMD(SUB, {}, {
    _LOG_EMPT_STACK_("SUB[top]");
    int arg_a = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);

    _LOG_EMPT_STACK_("SUB[bottom]");
    stack_content_t arg_b = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);

    stack_push(STACK, arg_a - arg_b);
}, {})

DEF_CMD(MUL, {}, {
    _LOG_EMPT_STACK_("MUL[top]");
    stack_content_t arg_a = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);

    _LOG_EMPT_STACK_("MUL[bottom]");
    stack_content_t arg_b = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);

    stack_push(STACK, arg_a * arg_b);
}, {})

DEF_CMD(DIV, {}, {
    _LOG_EMPT_STACK_("DIV[top]");
    stack_content_t arg_a = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);

    _LOG_EMPT_STACK_("DIV[bottom]");
    stack_content_t arg_b = (int)stack_get(STACK, ERRNO); stack_pop(STACK, ERRNO);

    stack_push(STACK, arg_a / arg_b);
}, {})