#include "abel.hpp"

int main(void)
{
	cout << "Abel VM." << endl;

	Bytecode code({}, {OP_ADD, OP_ADD, OP_ADD, OP_RETURN});

	auto VM = AbelVm();
	VM.registerBlob("add3", std::move(code));
	VM.replaceStack({1, 2, 3, 4});
	auto result = VM.executeBlob("add3");
	if (auto error = std::get_if<Errors>(&result))
		cout << "Error: " << (int)*error << endl;
	exit(0);
}
