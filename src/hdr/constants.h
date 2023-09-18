#pragma once
#include <cstdint>
#include <condition_variable>

typedef uint8_t byte;
typedef uint16_t word;

const int KEYS_SIZE = 16;

// the structure stores variables shared variables of threads
// needed to declare one instance and pass it to the threads by pointer
struct sync_vars {
    std::condition_variable input_cv;   // variables indicates that any key was pressed
    std::mutex input_mut; // mutex used in Fx0A opcode that wait until needed key pressed
    bool key_pressed;
};