#pragma once

#include <variant>

enum class Errors
{
	NoSuchBlob,
	InvalidOpCode,
	ProgramOverrun,
	StackUnderrun,
	NoStackFrame
};

using RunResult = std::variant<int, Errors>;
