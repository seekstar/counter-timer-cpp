cmake_minimum_required(VERSION 3.15)
project(counter-timer CXX)

if (NOT TARGET rusty-cpp)
	find_package(rusty-cpp CONFIG REQUIRED)
endif()

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include)
target_link_libraries(${PROJECT_NAME}
	INTERFACE
		rusty-cpp
)

install(DIRECTORY include/ DESTINATION "include")
