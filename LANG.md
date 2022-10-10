# List of commands

These are commands recognized by assembler program. Each command should be written in a separate line. Comments should be marked with the *#* symbol at the beginning of the line.

1. **END** - successfully end program execution.
2. **PUSH *argument (integer)*** - push argument to the stack.
3. **POP** - remove last element from the stack.
4. **OUT** - print lase element in the stack as a number.
5. **ADD** - remove last two elements from the stack and push their sum back.
6. **SUB** - remove last two elements from the stack and push their difference back.
7. **MUL** - remove last two elements from the stack and push their product back.
8. **DIV** - remove last two elements from the stack and push their quotient back.
9. **DUP** - duplicate last element of the stack.
10. **ABORT** - abort program execution with failure.
11. **HERE *argument (string)*** - declare label at ths position in code.
12. **JMP *argument (string, label name)*** - set execution pointer to the specified label.
13. **JMPG *argument (string, label name)*** - remove last two elements from the stack and, if the first one of them is greater then the other, perform jump to the specified label.
14. **RGET *argument (integer)*** - push value from specified register to the stack.
15. **RSET *argument (integer)*** - set value of the specified register to the value of the last stack element.
16. **OUTC** - print symbol coresponding (acc. to UTF-8) to the last element on the stack.