#include "cpu.h"

cpu_chip8::cpu_chip8() {
    mem = std::make_shared<memory>();
    mem->init_hex_digits();
    for(int i = 0; i < 16; i++) {
        pressed_keys[i] = 0;
    }

    PC = PROGRAM_START_AREA;
    SP = STACK_START_AREA;
    to_execute = 0;
    delay_timer = 0;
    sound_timer = 0;
    mem->load_rom();
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
    opcode_table[0x9] = std::bind(&cpu_chip8::SNE, &(*this));
    opcode_table[0xA] = std::bind(&cpu_chip8::LD_I, &(*this));
    opcode_table[0xB] = std::bind(&cpu_chip8::JP_V0, &(*this));
    opcode_table[0xC] = std::bind(&cpu_chip8::RND, &(*this));
    opcode_table[0xD] = std::bind(&cpu_chip8::DRW, &(*this));
    opcode_table[0xE] = std::bind(&cpu_chip8::SKP_or_SKNP, &(*this));
    opcode_table[0xF] = std::bind(&cpu_chip8::GRP_2, &(*this));
}

void cpu_chip8::init_sync_channel(std::shared_ptr<sync_vars> ch) {
    channel = ch;
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

void cpu_chip8::run() {
    while(to_execute > 0) {
        execute();
        to_execute--;
    }
}

void cpu_chip8::update_timers(int t) {
    if (t > delay_timer) { delay_timer = 0; }
    if (t > sound_timer) { sound_timer = 0; }
    if (delay_timer > 0) { delay_timer -= t; }
    if (sound_timer > 0) { sound_timer -= t; }
}

void cpu_chip8::update_to_execute(int t_e) {
    // if waits key -> don't execute commands
    if (!channel->wait_for_key) {
        to_execute += t_e;
    }
}

void cpu_chip8::notify_key_pressed() {
    if (channel->key_pressed) {
        Vx[channel->reg_to_store] = channel->key_code;
    }
}

// opcode functions
void cpu_chip8::CLS_or_RET() {
    // clear display
    if (low_instr == 0xE0) {
        for (int i = DISPLAY_START_AREA; i <= DISPLAY_END_AREA; i++) {
            mem->write(i, 0);
        }
    }   // return
    else if (low_instr == 0xEE) {
        PC = mem->read_w(SP);   // take value from top of stack
        SP -= 2;                // decrease stack pointer
    }
}

void cpu_chip8::JP() {
    if ((high_instr >> 4) == 0x1) {
        PC = instruction & 0x0FFF;
    }
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
            else { Vx[0xF] = 0; }
            break;
        case 0x5:   // Vx SUB Vy, set VF if Vx > Vy, reset otherwise
            if(Vx[id1] > Vx[id2]) { temp = 1; }
            else { temp = 0; }
            Vx[id1] -= Vx[id2];
            Vx[0xF] = temp;
            break;
        case 0x6:   // Vx SHR 1, set VF if least-significant bit is 1
            if ((Vx[id1] & 0x01) == 1) { temp = 1; }
            else { temp = 0; }
            Vx[id1] = Vx[id1] >> 1;
            Vx[0xF] = temp;
            break;
        case 0x7:   // Vx SUBN Xy
            Vx[id1] = Vx[id2] - Vx[id1];
            if (Vx[id1] < Vx[id2]) { Vx[0xF] = 1; }
            else { Vx[0xF] = 0; }
            break;
        case 0xE:   // Vx SHL 1, set VF if most-significant bit is 1
            if ((Vx[id1] & 0x80) == 0x80) { temp = 1; }
            else { temp = 0; }
            Vx[id1] = Vx[id1] << 1;
            Vx[0xF] = temp;
            break;
    }
}

void cpu_chip8::SNE() {
    // low nimble must be zero
    if ((low_instr & 0x0F) != 0) return;
    // skip next command if Vx != Vy
    if (Vx[high_instr & 0x0F] != Vx[low_instr >> 4]) {
        PC += 2;
    }
}

void cpu_chip8::LD_I() {
    // set I to nnn
    I = instruction & 0x0FFF;
}

void cpu_chip8::JP_V0() {
    // set PC to nnn + V0
    if ((high_instr >> 4) == 0xB) {
        PC = (instruction & 0x0FFF) + Vx[0x0];
    }
}

void cpu_chip8::RND() {
    // generate random number and logical AND with kk
    // should use srand(time(0)) for better randomize
    Vx[high_instr & 0x0F] = rand() % 256 & low_instr;
}

// cyclic left shift
uint64_t rotl64 (uint64_t n, unsigned int c) {
  const unsigned int mask = (CHAR_BIT * sizeof(n) - 1);
  c &= mask;
  return (n << c) | (n >> ((-c) & mask));
}

void cpu_chip8::DRW() {
    // draw sprite addressed by I with height n in pos(x, y)
    byte x = Vx[high_instr & 0x0F];
    byte y = Vx[low_instr >> 4];
    byte len = low_instr & 0x0F;

    uint64_t row = 0;
    word offset = 0;
    // variables used to check if any pixel was cleared
    uint64_t new_row = 0;
    uint64_t old_row = 0;
    bool need_to_set = false;
    for (int i = 0; i < len; i++) {
        // place sprite byte in row
        row = mem->read(I + i);
        // then cycle shift (56 - x) times to left
        // 56 because row variable stores memory byte value, so it starts from eighth bit from end
        row = rotl64(row, (SCREEN_WIDTH - x - 8));
        // and xor it with (start position + row_id) % height * row_size)
        offset = DISPLAY_START_AREA + (((y + i) % SCREEN_HEIGHT) * sizeof(uint64_t));
        old_row = mem->read_qw(offset);
        new_row = row ^ old_row;
        mem->write_qw(offset, new_row);
        // check if pixel was cleared
        if (old_row > (new_row & old_row)) need_to_set = true;
    }
    if (need_to_set) Vx[0xF] = 1;
}

void cpu_chip8::SKP_or_SKNP() {
    byte key = Vx[high_instr & 0x0F];
    switch(low_instr) { // skip instruction if key pressed
        case 0x9E:
        if(pressed_keys[key] == true) {
            PC += 2;
        }
        break;
        case 0xA1:      // skip instruction if key not pressed
        if(pressed_keys[key] == false) {
            PC += 2;
        }
        break;
    }
}

void cpu_chip8::GRP_2() {
    byte id = high_instr & 0x0F;
    byte counter = 0;   // value for cycle
    switch (low_instr)  // low_inst repesents second byte of opcode
    {
    case 0x07:  // set Vx to delay timer
        Vx[id] = delay_timer;
        break;
    case 0x0A:  // wait for any a key, store key value to Vx
        channel->wait_for_key = true;   // tell that need a key
        channel->reg_to_store = id;     // remember register where to store key code
        to_execute = 0;                 // halt processor until key press
        break;
    case 0x15:  // set delay timer to Vx
        delay_timer = Vx[id];
        break;
    case 0x18:  // set sound timer to Vx
        sound_timer = Vx[id];
        break;
    case 0x1E:  // add Vx to I
        I = I + Vx[id];
        break;
    case 0x29:  // I is set to address of hex digit stored in Vx
        I = HEX_DIGITS_START_AREA + Vx[id] * HEX_DIGITS_LENGTH;
        break;
    case 0x33:  // store BDC representation of Vx in memory addressed by I
        mem->write(I, Vx[id] / 100);
        mem->write(I + 1, (Vx[id] / 10) % 10);
        mem->write(I + 2, Vx[id] % 10);
        break;
    case 0x55:  // store registers V0 - Vx to memory addressed by I
        for (counter; counter <= id; counter++) {
            mem->write(I + counter, Vx[counter]);
        }
        break;
    case 0x65:  // load registers V0 - Vx from memory addressed by I
        for (counter; counter <= id; counter++) {
            Vx[counter] = mem->read(I + counter);
        }
        break;
    }
}

bool* cpu_chip8::get_pk_ptr() {
    return pressed_keys;
}

std::shared_ptr<memory> cpu_chip8::get_mem_ptr() {
    return mem;
}