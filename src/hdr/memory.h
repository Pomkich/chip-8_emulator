#pragma once
#include <memory>
#include "constants.h"

const int MAX_MEM_SIZE = 0xFFF;

class memory {
private:
    byte byte_arr[MAX_MEM_SIZE];
public:
    memory();
    byte read(word address);
    void write(word address, byte data);

    word read_w(word address);
    void write_w(word address, word data);
};