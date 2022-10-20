# List of commands

These are commands recognized by the assembler program. Each command should be written in a separate line. Comments should be marked with the *#* symbol at the beginning of the line.

1. **END** - successfully end program execution.
2. **POP** - remove last element from the stack.
3. **OUT** - print last element of the stack as a number.
4. **ADD** - remove last two elements from the stack and push their sum back.
5. **SUB** - remove last two elements from the stack and push their difference back.
6. **MUL** - remove last two elements from the stack and push their product back.
7. **DIV** - remove last two elements from the stack and push their quotient back.
8. **DUP** - duplicate last element of the stack.
9.  **ABORT** - abort program execution with failure.
10. **HERE *argument (string)*** - declare label at ths position in code.
11. **JMP *argument (string, label name)*** - set execution pointer to the specified label.
12. **JMPG *argument (string, label name)*** - remove last two elements from the stack and, if the first one of them is greater then the other, perform jump to the specified label.
13. **OUTC** - print symbol coresponding (acc. to UTF-8) to the last element on the stack.
14. **PUSH *source*** - push value to the stack from source (source can be integer, character (if marked with brackets), register in the form of RAX, RBX, RCX e.t.c. or RAM cell (index can be specified in figure brackets and can be the value in the register)).
15. **MOVE *dest*** - move top value from the stack to the destination (destination is specified in the same format as PUSH source).
16. **VSET** - set video memory cell at [last element of the stack] to the value of [previous to the last element in the stack]. One element from the stack gets deleted.
17. **VGET** - replace the last element of the stack with the value from the video memory stored at the specified index.
18. **CCLR** - clear the console.
19. **DRAW** - draw video memory content to the screen as ASCII-art.