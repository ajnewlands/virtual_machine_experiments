#pragma once

#include <variant>

enum class Errors
{
	NoSuchBlob,
	InvalidOpCode,
	ProgramOverrun,
	StackUnderrun,
	NoStackFrame,
	SlotOutOfRange
};

using RunResult = std::variant<int, Errors>;
