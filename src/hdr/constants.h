#pragma once
#include <cstdint>
#include <condition_variable>

typedef uint8_t byte;
typedef uint16_t word;

const int KEYS_SIZE = 16;
const int STACK_START_AREA = 0x0;
const int PROGRAM_START_AREA = 0x200;
const int DISPLAY_START_AREA = 0x100;
const int DISPLAY_END_AREA = 0x1FF;
const int HEX_DIGITS_START_AREA = 0x4D;
const int HEX_DIGITS_LENGTH = 5;

const int SCREEN_WIDTH = 64;
const int SCREEN_HEIGHT = 32;
const int X_SCALE = 8;
const int Y_SCALE = 8;

// the structure stores variables needed for Fx0A command
struct sync_vars {
    byte reg_to_store;
    bool wait_for_key;
    bool key_pressed;
    byte key_code;
};