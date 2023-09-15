#pragma once
#include "constants.h"
#include "memory.h"
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

    // variables stores current instruction
    byte low_instr, high_instr;
    // map stores opcode identifiers and pointers to functions for calling
    std::map<byte, std::function<void()>> opcode_table;
    // raw pointer to memory
    memory* mem;
public:
    cpu_chip8();
    void init_op_table();   // initialize opcode table 
    void execute();         // execute one command

private:
    void CLS_or_RET();
};