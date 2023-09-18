#pragma once
#include <cstdint>
#include <condition_variable>

typedef uint8_t byte;
typedef uint16_t word;

const int KEYS_SIZE = 16;

std::condition_variable input_cv;   // variables indicates that any key was pressed
std::mutex input_mut; // mutex used in Fx0A opcode that wait until needed key pressed
bool key_pressed;