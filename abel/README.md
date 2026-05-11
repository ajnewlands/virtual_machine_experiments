Abel is a trivial bytecode interpreter that doesn't try to do anything particular well or sophisticated.

It is implemented in C++ (17 required) mainly because I hadn't written any C/C++ in yonks (day job currently being Rust-centric) and wanted to see if I still could.

Abel is a stack machine which provides a stack per subroutine (i.e. a badly behaved callee cannot corrupt the callers stack).

Abel can only push/pop/add and call subroutines (with a fixed single return value required).

Abel isn't really useful for anything except playing with the most fundamental elements of a stack machine.
