#pragma once

#include <variant>
#include "errors.hpp"

enum OpCodes : u_int8_t
{
	/// @brief  Return from bytecode blob
	OP_RETURN = 0,
	/// @brief Pop top 2 elements, perform integer addition, push result.
	OP_ADD = 1,
	/// @brief Push the following quad long word to the stack
	OP_PUSH = 2,
	/// @brief  Pop a QLW from the stack, discarding it.
	OP_POP = 3,
	/// @brief Call the bytecode with the given index (read from bytecode)
	OP_CALL = 4,
};

using OpCodeResult = std::variant<OpCodes, Errors>;
