# Changes
## 1. alloc_tracker module
This module allows the user (programmer) to add their allocation to the tracking list (yes, it uses list data structure under the hood) and allow the module to automatically free mentioned allocations.
```
track_allocation(&variable, (dtor_t*)destructor);
 
...
 
free_all_allocations();
/* or */
free_allocation(&variable);
```
 
This module allowed for main() functions refactor as there is no longer need to write a bunch of free()-s and dtor()-s in every fail-case execution branch.
 
## 2. Moved command-line argument definitions to separate files
All command line argument defs are now in separate include-able files.
 
## 3. Got rid of global variables
Most of global variables (except constants) were replaced with local ones. Most changes are in main()-containing files.
 
## 4. Created separate files for miscellaneous functions
`util/` folder in `src/` contains utility functions used in main files that were previously written in each and every separate file.
 
## 5. Code example set update
Replaced old RGET/RSET and MGET/MSET functions with newer PUSH and MOVE functions.
 
## 6. Project structure update
Created `src` folder for, well, source files of the project that were earlier located in `lib`
 
## 7. Reduced PUSH/MOVE code volume
Most of the PUSH/MOVE logic was packed inside separate and more general utility functions.
 
## 8. Reduced sim. command volume
The same part of similar commands (like arithmetical commands and conditional jumps) was packed inside temporary macros.
Their definitions were also moved to separate files to pack similar logic together and separate table-like definitions with prefixes from the rest of the code.
 
## 9. "gradient.txt" code example update
The gradient code example now includes a black circle in the middle of the screen.
Output example:
```
nnxxrrjjfftt//\\||(())11{{}}[[]]??--__++~~<<>>ii!!llII;;::,,""^^
nnnxxrrjjfftt//\\||(())11{{}}[[]]??--__++~~<<>>ii!!llII;;::,,""^
unnnxxrrjjfftt//\\||(())11{{}}[[]]??--__++~~<<>>ii!!llII;;::,,""
uunnnxxrrjjfftt//\\||(())11{{}}[[]]??--__++~~<<>>ii!!llII;;::,,"
vuunnnxxrrjjfftt//\\||(())11{{}}[[]]??--__++~~<<>>ii!!llII;;::,,
vvuunnnxxrrjjfftt//\\||(())11{{}}[[]]??--__++~~<<>>ii!!llII;;::,
cvvuunnnxxrrjjfftt//\\||(())11{{}}[[]]??--__++~~<<>>ii!!llII;;::
ccvvuunnnxxrrjjfftt//\\                   -__++~~<<>>ii!!llII;;:
zccvvuunnnxxrrjjfftt/                       __++~~<<>>ii!!llII;;
zzccvvuunnnxxrrjj                               +~~<<>>ii!!llII;
Xzzccvvuunnnxxrrj                               ++~~<<>>ii!!llII
XXzzccvvuunnnxx                                   +~~<<>>ii!!llI
YXXzzccvvuunn                                       ~~<<>>ii!!ll
YYXXzzccvvuun                                       +~~<<>>ii!!l
UYYXXzzccvvuu                                       ++~~<<>>ii!!
UUYYXXzzccvvu                                       _++~~<<>>ii!
JUUYYXXzzccvv                                       __++~~<<>>ii
JJUUYYXXzzccv                                       -__++~~<<>>i
CJJUUYYXXzzcc                                       --__++~~<<>>
CCJJUUYYXXzzc                                       ?--__++~~<<>
LCCJJUUYYXXzz                                       ??--__++~~<<
LLCCJJUUYYXXzzc                                   []]??--__++~~<
QLLCCJJUUYYXXzzcc                               }}[[]]??--__++~~
QQLLCCJJUUYYXXzzc                               {}}[[]]??--__++~
0QQLLCCJJUUYYXXzzccvv                       ))11{{}}[[]]??--__++
00QQLLCCJJUUYYXXzzccvvu                   |(())11{{}}[[]]??--__+
O00QQLLCCJJUUYYXXzzccvvuunnnxxrrjjfftt//\\||(())11{{}}[[]]??--__
OO00QQLLCCJJUUYYXXzzccvvuunnnxxrrjjfftt//\\||(())11{{}}[[]]??--_
ZOO00QQLLCCJJUUYYXXzzccvvuunnnxxrrjjfftt//\\||(())11{{}}[[]]??--
ZZOO00QQLLCCJJUUYYXXzzccvvuunnnxxrrjjfftt//\\||(())11{{}}[[]]??-
mZZOO00QQLLCCJJUUYYXXzzccvvuunnnxxrrjjfftt//\\||(())11{{}}[[]]??
mmZZOO00QQLLCCJJUUYYXXzzccvvuunnnxxrrjjfftt//\\||(())11{{}}[[]]?
```
 
## 10. Created "quadratic.txt" code example
Added example program that reads three coefficients from the user and spits out solutions of the quadratic equations with specified coefficients. Output example:
```
A=1
B=-2
C=-3
X1=-1
X2=3
```
 
```
A=1
B=2
C=1
X1=-1
```
 
```
A=1
B=1
C=1
 
No solutions!
```
 
## 11. Added IN command
IN command can be used to read user input. There is nothing much to say about it except that it does not exist in most assembler-style languages.
 
## 12. Packed virtual RAM and register into structures
Now, RAM and register are entities of MemorySegment structure.

### TODO: Add code examples (esp. changes)
