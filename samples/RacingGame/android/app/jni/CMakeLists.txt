# SPDX-License-Identifier: MS-PL

cmake_minimum_required(VERSION 3.20)
project(RacingGameAndroid LANGUAGES CXX)

# The sample owns renderer selection and produces libmain.so through the same
# CMakeLists.txt used by its desktop fidelity harnesses.
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../.." racing_game_build)
