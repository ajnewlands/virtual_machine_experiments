#include <iostream>
#include <cstdio>
#include <boost/program_options.hpp>
#include "ast.hpp"
#include "codegen.hpp"
#include "abel.hpp"

namespace po = boost::program_options;

extern int yyparse();
extern FILE *yyin;
extern Program g_program;

struct Args {
	std::string input;
};

Args parse_args(int argc, char *argv[]) {
	po::options_description desc("abel-asm options");
	desc.add_options()
		("help", "Show help message")
		("input", po::value<std::string>()->required(), "Input assembly file");

	po::variables_map opts;
	po::store(po::parse_command_line(argc, argv, desc), opts);

	if (opts.count("help")) {
		std::cout << desc << std::endl;
		exit(0);
	}

	po::notify(opts);
	return {opts["input"].as<std::string>()};
}

int main(int argc, char *argv[]) {
	Args args;
	try {
		args = parse_args(argc, argv);
	} catch (const po::error &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	yyin = fopen(args.input.c_str(), "r");
	if (!yyin) {
		std::cerr << "Cannot open " << args.input << std::endl;
		return 1;
	}

	if (yyparse() != 0) {
		fclose(yyin);
		return 1;
	}
	fclose(yyin);

	auto outcome = codegen(g_program);
	if (auto *err = std::get_if<CodegenError>(&outcome)) {
		std::cerr << "Error: " << err->message << std::endl;
		return 1;
	}

	auto &result = std::get<CodegenResult>(outcome);

	AbelVm vm;
	for (auto &[name, blob] : result.blobs)
		vm.registerBlob(name, std::move(blob));

	auto run = vm.executeBlobByName(result.entrypoint);
	if (auto *err = std::get_if<Errors>(&run)) {
		std::cerr << "Runtime error: " << static_cast<int>(*err) << std::endl;
		return 1;
	}

	std::cout << "Exit: " << std::get<int>(run) << std::endl;
	return 0;
}
