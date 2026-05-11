#include "abel.hpp"

Bytecode::Bytecode(vector<u_int8_t> data, vector<u_int8_t> instructions)
{
	this->_data = std::move(data);
	this->_instructions = std::move(instructions);
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
		return static_cast<OpCodes>(byte);
	default:
		return Errors::InvalidOpCode;
	}
}

size_t AbelVm::registerBlob(string name, Bytecode blob)
{
	this->_blobs.push_back(make_unique<Bytecode>(std::move(blob)));
	size_t index = this->_blobs.size() - 1;
	this->_blobIndex.insert({name, index});
	return index;
}

void AbelVm::replaceStack(vector<u_int64_t> stack)
{
	this->_stack = std::move(stack);
}

RunResult AbelVm::executeBlob(string name)
{
	/// Check the internal collection for the named blob
	auto it = this->_blobIndex.find(name);
	if (it == this->_blobIndex.end())
		return Errors::NoSuchBlob;
	auto &target = this->_blobs[it->second];

	/// Start executing instructions until we hit OP_RETURN or run off the end of the world
	auto op = OpCodes::OP_RETURN;
	do
	{
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
			if (this->_stack.size() < 2)
				return Errors::StackUnderrun;
			uint64_t a = this->_stack.back();
			this->_stack.pop_back();
			uint64_t b = this->_stack.back();
			this->_stack.pop_back();
			this->_stack.push_back(a + b);
			cout << "Add: " << a << " + " << b << " = " << (a + b) << endl;
			break;
		}
		case OpCodes::OP_RETURN:
			break;
		default:
			return Errors::InvalidOpCode;
		}

	} while (op != OpCodes::OP_RETURN);

	return 0;
}
