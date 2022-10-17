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