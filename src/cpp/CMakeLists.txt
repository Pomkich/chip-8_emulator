add_executable(main main.cpp cpu.cpp)
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/src/hdr)