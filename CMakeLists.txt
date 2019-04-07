cmake_minimum_required (VERSION 3.9.0)

set(CMAKE_USER_MAKE_RULES_OVERRIDE_C ${CMAKE_CURRENT_SOURCE_DIR}/OverrideC.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX ${CMAKE_CURRENT_SOURCE_DIR}/OverrideCXX.cmake)

project(Ui_clicker_cpp LANGUAGES CXX C)

set(This Example)
project(${This} CXX)

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

add_subdirectory(Clicker)
