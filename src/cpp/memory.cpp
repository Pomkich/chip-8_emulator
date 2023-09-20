#include "memory.h"

memory::memory() {
    memset(byte_arr, 0, MAX_MEM_SIZE);
    //memset(byte_arr + DISPLAY_START_AREA, 0xFF, 0x100);
}

byte memory::read(word address) {
    if (address >= 0 && address < MAX_MEM_SIZE) {
        return byte_arr[address];
    }
    return 0;
}

void memory::write(word address, byte data) {
    if (address >= 0 && address < MAX_MEM_SIZE) {
        byte_arr[address] = data;
    }
}

word memory::read_w(word address) {
    if (address >= 0 && address < MAX_MEM_SIZE) {
        return ((word)byte_arr[address] << 8) | byte_arr[address + 1];
    }
    return 0;
}

void memory::write_w(word address, word data) {
    if (address >= 0 && address < MAX_MEM_SIZE) {
        byte_arr[address] = (data >> 8);
        byte_arr[address + 1] = data & 0x00FF;
    }
}

uint64_t memory::read_qw(word address) {
    if (address >= 0 && address < MAX_MEM_SIZE) {
        uint64_t res = 0;
        for (int i = 0; i < sizeof(uint64_t); i++) {
            res |= byte_arr[address + i];
            res = res << 8;
        }
        return res;
    }
    return 0;
}

void memory::write_qw(word address, uint64_t data) {
    if (address >= 0 && address <= MAX_MEM_SIZE) {
        for (int i = 0; i < sizeof(uint64_t); i++) {
            byte_arr[address + ((sizeof(uint64_t) - i))] = data & 0x00000000000000FF;
            data = data >> 8;
        }
    }
}

//"./test_opcode.ch8"
//./2-ibm-logo.ch8
//./3-corax+.ch8
void memory::load_test_rom() {
    std::ifstream rom("./3-corax+.ch8", std::ios::binary);
    if (rom) {
        std::cout << "file loaded" << std::endl;
    } 
    else {
        std::cout << "file not loaded" << std::endl;
    }
    rom.seekg(0, std::ios::end);
    int file_size = rom.tellg();
    rom.seekg(std::ios::beg);
    rom.unsetf(std::ios::skipws);
    for (int i = PROGRAM_START_AREA; i < PROGRAM_START_AREA + file_size; i++) {
        rom >> byte_arr[i];
    }
    rom.close();
}

void memory::init_hex_digits() {
    // ZERO
    byte_arr[HEX_DIGITS_START_AREA + 0] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 1] = 0x90;
    byte_arr[HEX_DIGITS_START_AREA + 2] = 0x90;
    byte_arr[HEX_DIGITS_START_AREA + 3] = 0x90;
    byte_arr[HEX_DIGITS_START_AREA + 4] = 0xF0;
    // ONE
    byte_arr[HEX_DIGITS_START_AREA + 5] = 0x20;
    byte_arr[HEX_DIGITS_START_AREA + 6] = 0x60;
    byte_arr[HEX_DIGITS_START_AREA + 7] = 0x20;
    byte_arr[HEX_DIGITS_START_AREA + 8] = 0x20;
    byte_arr[HEX_DIGITS_START_AREA + 9] = 0x70;
    // TWO
    byte_arr[HEX_DIGITS_START_AREA + 10] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 11] = 0x10;
    byte_arr[HEX_DIGITS_START_AREA + 12] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 13] = 0x80;
    byte_arr[HEX_DIGITS_START_AREA + 14] = 0xF0;
    // THREE
    byte_arr[HEX_DIGITS_START_AREA + 15] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 16] = 0x10;
    byte_arr[HEX_DIGITS_START_AREA + 17] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 18] = 0x10;
    byte_arr[HEX_DIGITS_START_AREA + 19] = 0xF0;
    // FOUR
    byte_arr[HEX_DIGITS_START_AREA + 20] = 0x90;
    byte_arr[HEX_DIGITS_START_AREA + 21] = 0x90;
    byte_arr[HEX_DIGITS_START_AREA + 22] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 23] = 0x10;
    byte_arr[HEX_DIGITS_START_AREA + 24] = 0x10;
    // FIVE
    byte_arr[HEX_DIGITS_START_AREA + 25] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 26] = 0x80;
    byte_arr[HEX_DIGITS_START_AREA + 27] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 28] = 0x10;
    byte_arr[HEX_DIGITS_START_AREA + 29] = 0xF0;
    // SIX
    byte_arr[HEX_DIGITS_START_AREA + 30] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 31] = 0x80;
    byte_arr[HEX_DIGITS_START_AREA + 32] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 33] = 0x90;
    byte_arr[HEX_DIGITS_START_AREA + 34] = 0xF0;
    // SEVEN
    byte_arr[HEX_DIGITS_START_AREA + 35] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 36] = 0x10;
    byte_arr[HEX_DIGITS_START_AREA + 37] = 0x20;
    byte_arr[HEX_DIGITS_START_AREA + 38] = 0x40;
    byte_arr[HEX_DIGITS_START_AREA + 39] = 0x40;
    // EIGHT
    byte_arr[HEX_DIGITS_START_AREA + 40] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 41] = 0x90;
    byte_arr[HEX_DIGITS_START_AREA + 42] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 43] = 0x90;
    byte_arr[HEX_DIGITS_START_AREA + 44] = 0xF0;
    // NINE
    byte_arr[HEX_DIGITS_START_AREA + 45] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 46] = 0x90;
    byte_arr[HEX_DIGITS_START_AREA + 47] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 48] = 0x10;
    byte_arr[HEX_DIGITS_START_AREA + 49] = 0xF0;
    // A
    byte_arr[HEX_DIGITS_START_AREA + 50] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 51] = 0x90;
    byte_arr[HEX_DIGITS_START_AREA + 52] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 53] = 0x90;
    byte_arr[HEX_DIGITS_START_AREA + 54] = 0x90;
    // B
    byte_arr[HEX_DIGITS_START_AREA + 55] = 0xE0;
    byte_arr[HEX_DIGITS_START_AREA + 56] = 0x90;
    byte_arr[HEX_DIGITS_START_AREA + 57] = 0xE0;
    byte_arr[HEX_DIGITS_START_AREA + 58] = 0x90;
    byte_arr[HEX_DIGITS_START_AREA + 59] = 0xE0;
    // C
    byte_arr[HEX_DIGITS_START_AREA + 60] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 61] = 0x80;
    byte_arr[HEX_DIGITS_START_AREA + 62] = 0x80;
    byte_arr[HEX_DIGITS_START_AREA + 63] = 0x80;
    byte_arr[HEX_DIGITS_START_AREA + 64] = 0xF0;
    // D
    byte_arr[HEX_DIGITS_START_AREA + 65] = 0xE0;
    byte_arr[HEX_DIGITS_START_AREA + 66] = 0x90;
    byte_arr[HEX_DIGITS_START_AREA + 67] = 0x90;
    byte_arr[HEX_DIGITS_START_AREA + 68] = 0x90;
    byte_arr[HEX_DIGITS_START_AREA + 69] = 0xE0;
    // E
    byte_arr[HEX_DIGITS_START_AREA + 70] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 71] = 0x80;
    byte_arr[HEX_DIGITS_START_AREA + 72] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 73] = 0x80;
    byte_arr[HEX_DIGITS_START_AREA + 74] = 0xF0;
    // F
    byte_arr[HEX_DIGITS_START_AREA + 75] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 76] = 0x80;
    byte_arr[HEX_DIGITS_START_AREA + 77] = 0xF0;
    byte_arr[HEX_DIGITS_START_AREA + 78] = 0x80;
    byte_arr[HEX_DIGITS_START_AREA + 79] = 0x80;
}