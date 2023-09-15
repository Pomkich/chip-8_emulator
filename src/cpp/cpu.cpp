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
    opcode_table[0x3] = std::bind(&cpu_chip8::SE_IMM, &(*this));
    opcode_table[0x4] = std::bind(&cpu_chip8::SNE_IMM, &(*this));
    opcode_table[0x5] = std::bind(&cpu_chip8::SE, &(*this));
    opcode_table[0x6] = std::bind(&cpu_chip8::LD, &(*this));
    opcode_table[0x7] = std::bind(&cpu_chip8::ADD, &(*this));
    opcode_table[0x8] = std::bind(&cpu_chip8::GRP_1, &(*this));
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
    // clear display
    if (low_instr == 0xE0) {
        std::cout << "display cleared" << std::endl;
    }   // return
    else if (low_instr == 0xEE) {
        PC = mem->read_w(SP);   // take value from top of stack
        SP -= 2;                // decrease stack pointer
    }
}

void cpu_chip8::JP() {
    PC = instruction & 0x0FFF;
}

void cpu_chip8::CALL() {
    SP += 2;                        // increment stack pointer
    mem->write_w(SP, PC);           // push program counter value to stack
    PC = instruction & 0x0FFF;      // set PC to new address
}

void cpu_chip8::SE_IMM() {
    // skip next command if Vx == kk
    if (Vx[high_instr & 0x0F] == low_instr) {
        PC += 2;
    }
}

void cpu_chip8::SNE_IMM() {
    // skip next command if Vx != kk
    if (Vx[high_instr & 0x0F] != low_instr) {
        PC += 2;
    }
}

void cpu_chip8::SE() {
    // low nimble must be zero
    if ((low_instr & 0x0F) != 0) return;
    // skip next command if Vx == Vy
    if (Vx[high_instr & 0x0F] == Vx[low_instr >> 4]) {
        PC += 2;
    }
}

void cpu_chip8::LD() {
    // put in Vx kk
    Vx[high_instr & 0x0F] = low_instr;
}

void cpu_chip8::ADD() {
    // add kk to Vx
    Vx[high_instr & 0x0F] += low_instr;
}

// group of register instructions
void cpu_chip8::GRP_1() {
    byte instruction_type = low_instr & 0x0F;
    byte id1 = high_instr & 0x0F;
    byte id2 = low_instr >> 4;
    byte temp = 0;  // temporary variable used for carry out check
    switch (instruction_type) {
        case 0x0:   // store Vy to Vx
            Vx[id1] = Vx[id2];
            break;
        case 0x1:   // Vx OR Vy
            Vx[id1] |= Vx[id2];
            break;
        case 0x2:   // Vx AND Vy
            Vx[id1] &= Vx[id2];
            break;
        case 0x3:   // Vx XOR Vy
            Vx[id1] ^= Vx[id2];
            break;
        case 0x4:   // Vx ADD Vy, set VF if carry
            temp = Vx[id1];
            Vx[id1] += Vx[id2];
            // if carry out -> set VF
            if (temp > Vx[id1]) { Vx[0xF] = 1; }
            break;
        case 0x5:   // Vx SUB Vy, set VF if Vx > Vy, reset otherwise
            if(Vx[id1] > Vx[id2]) { Vx[0xF] = 1; }
            else { Vx[0xF] = 0; }
            Vx[id1] -= Vx[id2];
            break;
        case 0x6:   // Vx SHR 1, set VF if least-significant bit is 1
            if ((Vx[id1] & 0x01) == 1) { Vx[0xF] = 1; }
            else { Vx[0xF] = 0; }
            Vx[id1] = Vx[id1] >> 1;
            break;
        case 0x7:   // Vx SUBN Xy
            if (Vx[id1] < Vx[id2]) { Vx[0xF] = 1; }
            else { Vx[0xF] = 0; }
            Vx[id1] = Vx[id2] - Vx[id1];
            break;
        case 0xE:   // Vx SHL 1, set VF if most-significant bit is 1
            if ((Vx[id1] & 0x80) == 1) { Vx[0xF] = 1; }
            else { Vx[0xF] = 0; }
            Vx[id1] = Vx[id1] << 1;
            break;
    }
}
