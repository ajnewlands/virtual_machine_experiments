#include "codegen.hpp"
#include "ops.hpp"
#include <cstring>

static void emit_u8(std::vector<uint8_t> &out, uint8_t v) {
	out.push_back(v);
}

static void emit_u64(std::vector<uint8_t> &out, uint64_t v) {
	uint8_t buf[8];
	memcpy(buf, &v, 8);
	out.insert(out.end(), buf, buf + 8);
}

CodegenOutcome codegen(const Program &program) {
	// Build constant table
	std::map<std::string, uint64_t> constants;
	for (auto &c : program.constants) {
		if (constants.count(c.name))
			return CodegenError{"Duplicate constant: " + c.name};
		constants[c.name] = c.value;
	}

	// Build function index
	std::map<std::string, size_t> func_index;
	for (size_t i = 0; i < program.functions.size(); i++) {
		auto &f = program.functions[i];
		if (func_index.count(f.name))
			return CodegenError{"Duplicate function: " + f.name};
		func_index[f.name] = i;
	}

	if (!func_index.count("_main"))
		return CodegenError{"Missing _main function"};

	CodegenResult result;
	result.entrypoint = "_main";

	for (auto &func : program.functions) {
		// Build slot map: args first, then vars
		std::map<std::string, uint8_t> slots;
		uint8_t slot = 0;
		for (auto &arg : func.args) {
			if (slots.count(arg))
				return CodegenError{"Duplicate slot name '" + arg + "' in " + func.name};
			slots[arg] = slot++;
		}
		for (auto &var : func.vars) {
			if (slots.count(var))
				return CodegenError{"Duplicate slot name '" + var + "' in " + func.name};
			slots[var] = slot++;
		}

		std::vector<uint8_t> code;

		// Emit preamble: push 0 for each var to reserve stack slots
		for (size_t i = 0; i < func.vars.size(); i++) {
			emit_u8(code, OP_PUSH);
			emit_u64(code, 0);
		}

		// Emit instructions
		for (auto &instr : func.instructions) {
			switch (instr.type) {
			case InstrType::PUSH: {
				emit_u8(code, OP_PUSH);
				// Try as constant first, then as literal
				if (constants.count(instr.operand)) {
					emit_u64(code, constants[instr.operand]);
				} else {
					// Must be a numeric literal (stored as string)
					uint64_t val = std::stoull(instr.operand);
					emit_u64(code, val);
				}
				break;
			}
			case InstrType::POP:
				emit_u8(code, OP_POP);
				break;
			case InstrType::ADD:
				emit_u8(code, OP_ADD);
				break;
			case InstrType::LOAD: {
				auto it = slots.find(instr.operand);
				if (it == slots.end())
					return CodegenError{"Undefined variable '@" + instr.operand + "' in " + func.name};
				emit_u8(code, OP_LOAD);
				emit_u8(code, it->second);
				break;
			}
			case InstrType::STORE: {
				auto it = slots.find(instr.operand);
				if (it == slots.end())
					return CodegenError{"Undefined variable '@" + instr.operand + "' in " + func.name};
				emit_u8(code, OP_STORE);
				emit_u8(code, it->second);
				break;
			}
			case InstrType::CALL: {
				auto it = func_index.find(instr.operand);
				if (it == func_index.end())
					return CodegenError{"Undefined function '" + instr.operand + "' in " + func.name};
				emit_u8(code, OP_CALL);
				emit_u64(code, it->second);
				break;
			}
			case InstrType::RET:
				emit_u8(code, OP_RETURN);
				break;
			}
		}

		uint8_t argc = static_cast<uint8_t>(func.args.size());
		result.blobs.emplace_back(func.name, Bytecode({}, std::move(code), argc));
	}

	return result;
}
