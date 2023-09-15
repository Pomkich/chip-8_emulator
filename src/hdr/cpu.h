#pragma once
#include "constants.h"
#include <iostream>
#include <map>
#include <functional>

class cpu_chip8 {
public:
    // general purpose registers
    // VF is flag register
    byte V0, V1, V2, V3;
    byte V4, V5, V6, V7;
    byte V8, V9, VA, VB;
    byte VC, VD, VE, VF;
    // register used for storing memory address
    word I;
    // timer registers
    byte delay_timer, sound_timer;
    // program counter
    word PC;
    // stack pointer
    word SP;

    // variable stores current instruction
    word instruction;
    // map stores opcode identifiers and pointers to functions for calling
    std::map<byte, std::function<void()>> opcode_table;
public:
    cpu_chip8();
    void init_op_table();
    void execute();

private:
    void CLS_or_RET();
};