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