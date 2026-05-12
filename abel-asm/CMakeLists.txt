cmake_minimum_required(VERSION 3.28)
project(abel-asm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

# Generate lexer and parser
BISON_TARGET(parser src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp
	DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hpp)
FLEX_TARGET(lexer src/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
ADD_FLEX_BISON_DEPENDENCY(lexer parser)

# libabel from sibling project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../abel ${CMAKE_CURRENT_BINARY_DIR}/abel)

# Main assembler executable
add_executable(abel-asm
	src/main.cpp
	src/codegen.cpp
	${BISON_parser_OUTPUTS}
	${FLEX_lexer_OUTPUTS}
)
target_include_directories(abel-asm PRIVATE
	src
	${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(abel-asm PRIVATE libabel Boost::program_options)

# Unit tests
add_executable(abel-asm-tests
	test/test_codegen.cpp
	src/codegen.cpp
)
target_include_directories(abel-asm-tests PRIVATE
	src
	${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(abel-asm-tests PRIVATE libabel)
