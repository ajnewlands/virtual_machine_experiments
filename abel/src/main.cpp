#include "abel.hpp"

int main(void)
{
	cout << "Abel VM." << endl;

	auto VM = AbelVm();
	/// Add3 is effectively fn add3(a, b, c, d) -> u64 {
	///     return a + b + c + d;
	/// }
	Bytecode add3({}, {OP_ADD, OP_ADD, OP_ADD, OP_RETURN}, 4);
	// add3 will be index 0
	VM.registerBlob("add3", std::move(add3));

	/// push 4, 3, 2, 1 and call add3 (NB endianness)
	vector<u_int8_t> entrypoint_code = {
		OP_PUSH, 4, 0, 0, 0, 0, 0, 0, 0,
		OP_PUSH, 3, 0, 0, 0, 0, 0, 0, 0,
		OP_PUSH, 2, 0, 0, 0, 0, 0, 0, 0,
		OP_PUSH, 1, 0, 0, 0, 0, 0, 0, 0,
		OP_CALL, 0, 0, 0, 0, 0, 0, 0, 0,
		OP_POP,
		OP_RETURN};
	Bytecode entrypoint({}, entrypoint_code, 0);
	// entrypoint will be index 1
	VM.registerBlob("entrypoint", std::move(entrypoint));
	auto result = VM.executeBlobByName("entrypoint");
	if (auto error = std::get_if<Errors>(&result))
		cout << "Error: " << (int)*error << endl;
	exit(0);
}
