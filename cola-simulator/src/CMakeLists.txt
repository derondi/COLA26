# minimum version of cmake
cmake_minimum_required (VERSION 3.1)

# project name
project (ldpc_exe)

add_subdirectory(common)
add_subdirectory(encoder)
add_subdirectory(gc-encoder)
add_subdirectory(failure)
add_subdirectory(gc-failure)
add_subdirectory(decoder)
add_subdirectory(gc-decoder)
add_subdirectory(util)

aux_source_directory(. DIR_SRCS)
add_executable(LDPCTest LDPCTest.cc)
add_executable(Hybrid-GC-LDPCTest Hybrid-GC-LDPCTest.cc)
add_executable(Local-GC-LDPCTest Local-GC-LDPCTest.cc)
add_executable(Global-GC-LDPCTest Global-GC-LDPCTest.cc)
#add_executable(OtherTest OtherTest.cc)

target_link_libraries(LDPCTest common util encoder failure decoder)
target_link_libraries(Hybrid-GC-LDPCTest common util gc-failure gc-decoder gc-encoder)
target_link_libraries(Local-GC-LDPCTest common util gc-failure gc-decoder gc-encoder)
target_link_libraries(Global-GC-LDPCTest common util gc-failure gc-decoder gc-encoder)

