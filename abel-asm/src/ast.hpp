#pragma once

#include <string>
#include <vector>
#include <cstdint>

enum class InstrType {
	PUSH, POP, ADD, LOAD, STORE, CALL, RET
};

struct Instruction {
	InstrType type;
	std::string operand; // variable name, function name, or literal
};

struct Function {
	std::string name;
	std::vector<std::string> args;
	std::vector<std::string> vars;
	std::vector<Instruction> instructions;
};

struct Constant {
	std::string name;
	uint64_t value;
};

struct Program {
	std::vector<Constant> constants;
	std::vector<Function> functions;
};
