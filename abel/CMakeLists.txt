cmake_minimum_required(VERSION 3.28)
project(abel)

add_executable(abel)

target_sources(abel
	PRIVATE
		src/main.cpp
		src/abel.cpp
)
