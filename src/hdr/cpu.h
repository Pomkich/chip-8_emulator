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
    byte Vx[0xF];
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
    word instruction;
    // map stores opcode identifiers and pointers to functions for calling
    std::map<byte, std::function<void()>> opcode_table;
    // raw pointer to memory
    memory* mem;
    // array stores current pressed keys
    bool pressed_keys[KEYS_SIZE];
    // structure for thread variables
    sync_vars* channel;
public:
    cpu_chip8();
    void init_op_table();   // initialize opcode table
    void init_sync_channel(sync_vars* ch); 
    void execute();         // execute one command
    void run();

private:
    // opcodes
    void CLS_or_RET();
    void JP();
    void CALL();
    void SE_IMM();
    void SNE_IMM();
    void SE();
    void LD();
    void ADD();
    void GRP_1();
    void SNE();
    void LD_I();
    void JP_V0();
    void RND();
    void DRW();
    void SKP_or_SKNP();
    void GRP_2();
public:
    bool* get_pk_ptr();
};