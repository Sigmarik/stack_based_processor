#define __COND_JMP_ASM { \
    char lbl_name[LABEL_MAX_NAME_LENGTH] = ""; \
    int argument = 0; \
 \
    sscanf(ARG_PTR, "%d", &argument); \
 \
    if (argument == 0) { \
        sscanf(ARG_PTR, "%s", lbl_name); \
        hash_t lbl_hash = get_hash(lbl_name, lbl_name + strlen(lbl_name)); \
        argument = (int)get_label(LABEL_LIST, lbl_hash, ERRNO) - (int)CUR_ID; \
    } \
 \
    BUF_WRITE(&argument, sizeof(argument)); \
}

#define __COND_JMP_RUN(comparator) { \
    int dest = 0; \
    memcpy(&dest, ARG_PTR, sizeof(dest)); \
 \
    _LOG_FAIL_CHECK_(dest != 0, "error", ERROR_REPORTS, { \
        log_printf(ERROR_REPORTS, "error", "JMPG argument was 0, terminating.\n"); \
    }, ERRNO, EFAULT); \
 \
    GET_TOP(stack_content_t arg_a); POP_TOP(); \
    GET_TOP(stack_content_t arg_b); POP_TOP(); \
 \
    if (arg_a comparator arg_b) SHIFT = dest; \
    else SHIFT += (int)sizeof(dest); \
}

#define __COND_JMP_DISASM { \
    int dest = 0; \
    memcpy(&dest, ARG_PTR, sizeof(dest)); \
 \
    SHIFT += (int)sizeof(dest); \
 \
    fprintf(OUT_FILE, "%d", dest); \
}

DEF_CMD(JMPG,  __COND_JMP_ASM, __COND_JMP_RUN( >), __COND_JMP_DISASM)
DEF_CMD(JMPL,  __COND_JMP_ASM, __COND_JMP_RUN( <), __COND_JMP_DISASM)
DEF_CMD(JMPE,  __COND_JMP_ASM, __COND_JMP_RUN(==), __COND_JMP_DISASM)
DEF_CMD(JMPGE, __COND_JMP_ASM, __COND_JMP_RUN(>=), __COND_JMP_DISASM)
DEF_CMD(JMPLE, __COND_JMP_ASM, __COND_JMP_RUN(<=), __COND_JMP_DISASM)

#undef __COND_JMP_ASM
#undef __COND_JMP_RUN
#undef __COND_JMP_DISASM