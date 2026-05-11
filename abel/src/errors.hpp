#pragma once

#include <variant>

enum class Errors
{
	NoSuchBlob,
	InvalidOpCode,
	ProgramOverrun,
	StackUnderrun
};

using RunResult = std::variant<int, Errors>;
