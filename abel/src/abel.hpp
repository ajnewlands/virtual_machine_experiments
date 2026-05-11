#pragma once

#include <iostream>
#include <memory>
#include <map>
#include <vector>

#include "errors.hpp"
#include "ops.hpp"

using namespace std;

class Bytecode
{
private:
	/// data constants e.g. strings
	vector<u_int8_t> _data;
	/// instructions stream
	vector<u_int8_t> _instructions;

public:
	Bytecode() = delete;
	Bytecode(Bytecode &&) = default;

	/// Construct from raw parts
	Bytecode(vector<u_int8_t> data, vector<u_int8_t> instructions);

	OpCodeResult readOpCode(size_t i);
};

class AbelVm
{
private:
	vector<unique_ptr<Bytecode>> _blobs;
	map<string, size_t> _blobIndex;
	/// @brief instruction pointer.
	u_int64_t ip = 0;
	/// The stack; each item is 64 bits.
	vector<uint64_t> _stack;

public:
	size_t registerBlob(string name, Bytecode blob);
	void replaceStack(vector<u_int64_t> stack);
	RunResult executeBlob(string name);
};
