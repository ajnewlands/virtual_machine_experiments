# Virtual Machine Experiments

Home to some experiments noodling about with virtual machines intended to act as bytecode interpreters for scripting languages.

[abel](abel/README.md)

Abel is the simplest and dumbest experiment. Abel is a stack machine capable of doing integer addition and calling subroutines. All subroutines return a stack element regardless of the actual return value (i.e. it could be a pointer, or a zero value for a void return). All functions report their "arity", so that the appropriate number of elements can be sliced off the caller stack, and planted in the callee stack (i.e. subroutines benefit from stack isolation).
