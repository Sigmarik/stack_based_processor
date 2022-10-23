DEF_CMD(OUTC, {}, {
    _LOG_EMPT_STACK_("OUT");
    putc((int)stack_get(STACK, ERRNO), stdout);
}, {})

DEF_CMD(OUT, {}, {
    _LOG_EMPT_STACK_("OUT");
    printf("%lld", stack_get(STACK, ERRNO));
}, {})

DEF_CMD(CCLR, {}, {
    clear_console(); //* Defined in processor.cpp
}, {})

DEF_CMD(DRAW, {}, {
    draw_vmd(&VMD); //* Defined in processor.cpp
}, {})