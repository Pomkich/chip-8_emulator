#include "cpu.h"

cpu_chip8::cpu_chip8() {
    
}

void cpu_chip8::init_op_table() {
    opcode_table[0x0] = std::bind(&cpu_chip8::CLS_or_RET, &(*this));
}

void cpu_chip8::execute() {
    instruction = 0x00E0;
    opcode_table[0x0]();
    instruction = 0x00EE;
    opcode_table[0x0]();
}

// opcode functions
void cpu_chip8::CLS_or_RET() {
    // crear display
    if ((instruction & 0x00FF) == 0xE0) {
        std::cout << "display cleared" << std::endl;
    }   // return
    else if ((instruction & 0x00FF) == 0xEE) {
        std::cout << "return called" << std::endl;
    }

}