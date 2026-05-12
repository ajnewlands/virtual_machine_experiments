#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include "ast.hpp"
#include "abel.hpp"

struct CodegenError {
	std::string message;
};

struct CodegenResult {
	std::vector<std::pair<std::string, Bytecode>> blobs;
	std::string entrypoint;
};

using CodegenOutcome = std::variant<CodegenResult, CodegenError>;

CodegenOutcome codegen(const Program &program);
