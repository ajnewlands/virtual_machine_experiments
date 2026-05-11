#pragma once

#include <variant>
#include "errors.hpp"

enum OpCodes : u_int8_t
{
	/// @brief  Return from bytecode blob
	OP_RETURN = 0,
	/// @brief Pop top 2 elements, perform integer addition, push result.
	OP_ADD = 1,
};

using OpCodeResult = std::variant<OpCodes, Errors>;
