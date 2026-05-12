#include "abel.hpp"
#include <cstring>

Bytecode::Bytecode(vector<u_int8_t> data, vector<u_int8_t> instructions, u_int8_t argc)
{
	this->_data = std::move(data);
	this->_instructions = std::move(instructions);
	this->_argc = argc;
}

OpCodeResult Bytecode::readOpCode(size_t i)
{
	if (i >= this->_instructions.size())
		return Errors::ProgramOverrun;

	u_int8_t byte = this->_instructions[i];
	switch (byte)
	{
	case OP_RETURN:
	case OP_ADD:
	case OP_PUSH:
	case OP_POP:
	case OP_CALL:
	case OP_LOAD:
	case OP_STORE:
		return static_cast<OpCodes>(byte);
	default:
		return Errors::InvalidOpCode;
	}
}

const u_int8_t *Bytecode::readBytes(size_t i, size_t count) const
{
	if (i + count > this->_instructions.size())
		return nullptr;
	return &this->_instructions[i];
}

size_t AbelVm::registerBlob(string name, Bytecode blob)
{
	this->_blobs.push_back(make_unique<Bytecode>(std::move(blob)));
	size_t index = this->_blobs.size() - 1;
	this->_blobIndex.insert({name, index});
	return index;
}

RunResult AbelVm::executeBlobByName(string name)
{
	auto it = this->_blobIndex.find(name);
	if (it == this->_blobIndex.end())
		return Errors::NoSuchBlob;
	this->_stackFrames.push_back({});
	return this->executeBlob(it->second);
}

RunResult AbelVm::executeBlob(size_t index)
{
	auto &target = this->_blobs[index];
	/// instruction pointer.
	u_int64_t ip = 0;

	/// Start executing instructions until we hit OP_RETURN or run off the end of the world
	auto op = OpCodes::OP_RETURN;
	do
	{
		if (this->_stackFrames.empty())
			return Errors::NoStackFrame;
		auto &stack = this->_stackFrames.back();
		OpCodeResult r = target->readOpCode(ip++);
		if (auto error = std::get_if<Errors>(&r))
			return *error;
		else
			op = std::get<OpCodes>(r);
		cout << "Executing OpCode " << (int)op << endl;

		switch (op)
		{
		case OpCodes::OP_ADD:
		{
			if (stack.size() < 2)
				return Errors::StackUnderrun;
			uint64_t a = stack.back();
			stack.pop_back();
			uint64_t b = stack.back();
			stack.pop_back();
			stack.push_back(a + b);
			cout << "Add: " << a << " + " << b << " = " << (a + b) << endl;
			break;
		}
		case OpCodes::OP_PUSH:
		{
			const u_int8_t *bytes = target->readBytes(ip, 8);
			if (!bytes)
				return Errors::ProgramOverrun;
			uint64_t value;
			memcpy(&value, bytes, 8);
			stack.push_back(value);
			cout << "Push: 0x" << hex << value << dec << endl;
			ip += 8;
			break;
		}
		case OpCodes::OP_POP:
		{
			if (stack.empty())
				return Errors::StackUnderrun;
			uint64_t a = stack.back();
			stack.pop_back();
			cout << "Pop: 0x" << hex << a << dec << endl;
			break;
		}
		case OpCodes::OP_CALL:
		{
			const u_int8_t *bytes = target->readBytes(ip, 8);
			if (!bytes)
				return Errors::ProgramOverrun;
			uint64_t callee;
			memcpy(&callee, bytes, 8);
			ip += 8;

			if (this->_blobs.size() < callee)
				return Errors::NoSuchBlob;

			u_int8_t argc = this->_blobs[callee]->argc();
			if (stack.size() < argc)
				return Errors::StackUnderrun;

			// Slice off argc elements from current stack into new frame
			stackFrame newFrame(stack.end() - argc, stack.end());
			stack.erase(stack.end() - argc, stack.end());
			this->_stackFrames.push_back(std::move(newFrame));

			cout << "Calling " << callee << endl;
			auto r = this->executeBlob(callee);
			if (auto error = std::get_if<Errors>(&r))
				return *error;

			// Pop return value, pop frame, push return value onto caller's frame
			uint64_t retVal = static_cast<uint64_t>(std::get<int>(r));
			this->_stackFrames.pop_back();
			this->_stackFrames.back().push_back(retVal);

			break;
		}
		case OpCodes::OP_LOAD:
		{
			const u_int8_t *bytes = target->readBytes(ip, 1);
			if (!bytes)
				return Errors::ProgramOverrun;
			u_int8_t slot = bytes[0];
			ip += 1;
			if (slot >= stack.size())
				return Errors::SlotOutOfRange;
			stack.push_back(stack[slot]);
			cout << "Load: slot " << (int)slot << " = 0x" << hex << stack[slot] << dec << endl;
			break;
		}
		case OpCodes::OP_STORE:
		{
			const u_int8_t *bytes = target->readBytes(ip, 1);
			if (!bytes)
				return Errors::ProgramOverrun;
			u_int8_t slot = bytes[0];
			ip += 1;
			if (stack.empty())
				return Errors::StackUnderrun;
			uint64_t value = stack.back();
			stack.pop_back();
			if (slot >= stack.size())
				return Errors::SlotOutOfRange;
			stack[slot] = value;
			cout << "Store: slot " << (int)slot << " = 0x" << hex << value << dec << endl;
			break;
		}
		case OpCodes::OP_RETURN:
			break;
		default:
			return Errors::InvalidOpCode;
		}

	} while (op != OpCodes::OP_RETURN);

	cout << "Returning from " << index << endl;

	auto &stack = this->_stackFrames.back();
	int retVal = stack.empty() ? 0 : static_cast<int>(stack.back());
	return retVal;
}
