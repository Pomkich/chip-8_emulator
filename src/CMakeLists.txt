add_executable(main 
    ./cpp/main.cpp 
    ./cpp/cpu.cpp 
    ./cpp/memory.cpp
)
target_include_directories(main PRIVATE ./hdr)