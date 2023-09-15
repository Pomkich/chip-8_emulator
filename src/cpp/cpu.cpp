#include "cpu.h"

cpu_chip8::cpu_chip8() {
    mem = new memory();

    PC = 0;
    mem->write(0x000, 0x00);
    mem->write(0x001, 0xE0);
    mem->write(0x002, 0x00);
    mem->write(0x003, 0xEE);
}

void cpu_chip8::init_op_table() {
    opcode_table[0x00] = std::bind(&cpu_chip8::CLS_or_RET, &(*this));
}

void cpu_chip8::execute() {
    high_instr = mem->read(PC++);
    low_instr = mem->read(PC++);
    instruction = high_instr << 8 | low_instr;

    opcode_table[high_instr]();
}

// opcode functions
void cpu_chip8::CLS_or_RET() {
    // crear display
    if (low_instr == 0xE0) {
        std::cout << "display cleared" << std::endl;
    }   // return
    else if (low_instr == 0xEE) {
        std::cout << "return called" << std::endl;
    }
}