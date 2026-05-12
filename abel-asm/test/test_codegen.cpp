#define BOOST_TEST_MODULE AbelAsmCodegen
#include <boost/test/included/unit_test.hpp>
#include "ast.hpp"
#include "codegen.hpp"

BOOST_AUTO_TEST_CASE(missing_main) {
	Program p;
	Function f;
	f.name = "foo";
	p.functions.push_back(f);

	auto outcome = codegen(p);
	auto *err = std::get_if<CodegenError>(&outcome);
	BOOST_REQUIRE(err != nullptr);
	BOOST_CHECK(err->message.find("_main") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(duplicate_constant) {
	Program p;
	p.constants.push_back({"X", 1});
	p.constants.push_back({"X", 2});
	Function f;
	f.name = "_main";
	p.functions.push_back(f);

	auto outcome = codegen(p);
	auto *err = std::get_if<CodegenError>(&outcome);
	BOOST_REQUIRE(err != nullptr);
	BOOST_CHECK(err->message.find("Duplicate constant") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(undefined_variable) {
	Program p;
	Function f;
	f.name = "_main";
	f.instructions.push_back({InstrType::LOAD, "nonexistent"});
	p.functions.push_back(f);

	auto outcome = codegen(p);
	auto *err = std::get_if<CodegenError>(&outcome);
	BOOST_REQUIRE(err != nullptr);
	BOOST_CHECK(err->message.find("Undefined variable") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(undefined_function_call) {
	Program p;
	Function f;
	f.name = "_main";
	f.instructions.push_back({InstrType::CALL, "ghost"});
	p.functions.push_back(f);

	auto outcome = codegen(p);
	auto *err = std::get_if<CodegenError>(&outcome);
	BOOST_REQUIRE(err != nullptr);
	BOOST_CHECK(err->message.find("Undefined function") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(simple_program_generates_blobs) {
	Program p;
	p.constants.push_back({"VAL", 42});

	Function main_fn;
	main_fn.name = "_main";
	main_fn.vars.push_back("x");
	main_fn.instructions.push_back({InstrType::PUSH, "VAL"});
	main_fn.instructions.push_back({InstrType::STORE, "x"});
	main_fn.instructions.push_back({InstrType::LOAD, "x"});
	main_fn.instructions.push_back({InstrType::RET, ""});
	p.functions.push_back(main_fn);

	auto outcome = codegen(p);
	auto *result = std::get_if<CodegenResult>(&outcome);
	BOOST_REQUIRE(result != nullptr);
	BOOST_CHECK_EQUAL(result->blobs.size(), 1);
	BOOST_CHECK_EQUAL(result->entrypoint, "_main");
}
