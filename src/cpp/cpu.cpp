#include "cpu.h"

cpu_chip8::cpu_chip8() {
    mem = new memory();

    PC = 0x00;
    SP = 0x100;
    mem->write(0x000, 0x22);
    mem->write(0x001, 0x00);
    mem->write(0x200, 0x00);
    mem->write(0x201, 0xEE);
}

void cpu_chip8::init_op_table() {
    opcode_table[0x0] = std::bind(&cpu_chip8::CLS_or_RET, &(*this));
    opcode_table[0x1] = std::bind(&cpu_chip8::JP, &(*this));
    opcode_table[0x2] = std::bind(&cpu_chip8::CALL, &(*this));
}

void cpu_chip8::execute() {
    high_instr = mem->read(PC++);
    low_instr = mem->read(PC++);
    // assembly in one instruction
    instruction = high_instr;
    instruction = instruction << 8;
    instruction = instruction | low_instr;
    // check only first 4 bits - they identify opcode
    opcode_table[(high_instr & 0xF0) >> 4]();
}

// opcode functions
void cpu_chip8::CLS_or_RET() {
    // crear display
    if (low_instr == 0xE0) {
        std::cout << "display cleared" << std::endl;
    }   // return
    else if (low_instr == 0xEE) {
        PC = mem->read_w(SP);   // take value from top of stack
        SP -= 2;                // decrease stack pointer
        std::cout << PC << std::endl;
    }
}

void cpu_chip8::JP() {
    PC = instruction & 0x0FFF;
}

void cpu_chip8::CALL() {
    SP += 2;                        // increment stack pointer
    mem->write_w(SP, PC);           // push program counter value to stack
    PC = instruction & 0x0FFF;      // set PC to new address
    std::cout << PC << std::endl;
}
