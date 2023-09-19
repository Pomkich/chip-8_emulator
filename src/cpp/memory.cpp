#include "memory.h"

memory::memory() {
    memset(byte_arr, 0, MAX_MEM_SIZE);
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
    if (address >= 0 && address < MAX_MEM_SIZE) {
        for (int i = 0; i < sizeof(uint64_t); i++) {
            byte_arr[address + (sizeof(uint64_t) - i)] = data & 0x00000000000000FF;
            data = data >> 8;
        }
    }
}
