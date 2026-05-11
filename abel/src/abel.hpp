#pragma once

#include <iostream>
#include <memory>
#include <map>
#include <vector>

#include "errors.hpp"
#include "ops.hpp"

using namespace std;

using stackFrame = vector<u_int64_t>;

class Bytecode
{
private:
	/// data constants e.g. strings
	vector<u_int8_t> _data;
	/// instructions stream
	vector<u_int8_t> _instructions;
	/// How many arguments this function will consume from the stack.
	u_int8_t _argc;

public:
	Bytecode() = delete;
	Bytecode(Bytecode &&) = default;

	/// Construct from raw parts
	Bytecode(vector<u_int8_t> data, vector<u_int8_t> instructions, u_int8_t argc);

	OpCodeResult readOpCode(size_t i);
	const u_int8_t *readBytes(size_t i, size_t count) const;
	u_int8_t argc() const { return _argc; }
};

class AbelVm
{
private:
	vector<unique_ptr<Bytecode>> _blobs;
	map<string, size_t> _blobIndex;
	/// The stack; each item is 64 bits.
	vector<stackFrame> _stackFrames;

	RunResult executeBlob(size_t index);

public:
	size_t registerBlob(string name, Bytecode blob);
	RunResult executeBlobByName(string name);
};
