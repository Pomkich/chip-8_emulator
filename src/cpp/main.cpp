#include <iostream>
#include "cpu.h"
#include "memory.h"

using namespace std;

int main() {
    cpu_chip8 my_cpu;
    std::cout << "cpu created" << std::endl;
    memory mem;
    std::cout << "memory created" << std::endl;
    mem.write(0x200, 0x20);
    std::cout << mem.read(0x200) << std::endl;
    return 0;
}