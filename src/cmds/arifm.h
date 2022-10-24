#define __STACK_ELEM_OPERATION(operation) { \
    GET_TOP(stack_content_t arg_a); POP_TOP(); \
    GET_TOP(stack_content_t arg_b); POP_TOP(); \
    PUSH(arg_a operation arg_b); \
}

DEF_CMD(ADD, {}, __STACK_ELEM_OPERATION(+), {})

DEF_CMD(SUB, {}, __STACK_ELEM_OPERATION(-), {})

DEF_CMD(MUL, {}, __STACK_ELEM_OPERATION(*), {})

DEF_CMD(DIV, {}, __STACK_ELEM_OPERATION(/), {})