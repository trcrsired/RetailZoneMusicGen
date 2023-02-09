cmake_minimum_required(VERSION 3.22)

project(RetailZoneMusicGen)

set(CMAKE_CXX_STANDARD 23)

add_executable(RetailZoneMusicGen main.cc mp3_duration.cc)
target_precompile_headers(RetailZoneMusicGen PRIVATE pch.hpp)
target_include_directories(RetailZoneMusicGen PRIVATE ${FAST_IO_DIR}/include)

install(TARGETS RetailZoneMusicGen)
