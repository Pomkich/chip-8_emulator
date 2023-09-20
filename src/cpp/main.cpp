#include "constants.h"
#include "cpu.h"
#include "memory.h"
#include <SFML/Graphics.hpp>
#include <thread>

std::map<sf::Keyboard::Key, byte> init_index_table() {
    std::map<sf::Keyboard::Key, byte> index_table;
    index_table[sf::Keyboard::Key::Num0] = 0x0;
    index_table[sf::Keyboard::Key::Num1] = 0x1;
    index_table[sf::Keyboard::Key::Num2] = 0x2;
    index_table[sf::Keyboard::Key::Num3] = 0x3;
    index_table[sf::Keyboard::Key::Num4] = 0x4;
    index_table[sf::Keyboard::Key::Num5] = 0x5;
    index_table[sf::Keyboard::Key::Num6] = 0x6;
    index_table[sf::Keyboard::Key::Num7] = 0x7;
    index_table[sf::Keyboard::Key::Num8] = 0x8;
    index_table[sf::Keyboard::Key::Num9] = 0x9;
    index_table[sf::Keyboard::Key::A] = 0xA;
    index_table[sf::Keyboard::Key::B] = 0xB;
    index_table[sf::Keyboard::Key::C] = 0xC;
    index_table[sf::Keyboard::Key::D] = 0xD;
    index_table[sf::Keyboard::Key::E] = 0xE;
    index_table[sf::Keyboard::Key::F] = 0xF;
}

// it's slow to check every pixel every tick, so i want to construct
// data structure from which i can take fragments of 8 pixels 
std::map<byte, sf::Uint8*> init_pixel_table() {
    std::map<byte, sf::Uint8*> res;
    byte array_id = 0;
    for (int i = 0; i < 256; i++) {
        res[i] = new sf::Uint8[32]; // every pixel stored as 4 bytes
        for (int pixel_id = 0; pixel_id < 8; pixel_id++) {
            array_id = (7 - pixel_id);  // need to translate original pixel to place in array
            if ((i >> pixel_id) & 0b00000001) { // white pixel
                res[i][array_id * 4 + 0] = 0xFF;
                res[i][array_id * 4 + 1] = 0xFF;
                res[i][array_id * 4 + 2] = 0xFF;
                res[i][array_id * 4 + 3] = 0x00;
            }
            else {  //black pixel
                res[i][array_id * 4 + 0] = 0x00;
                res[i][array_id * 4 + 1] = 0x00;
                res[i][array_id * 4 + 2] = 0x00;
                res[i][array_id * 4 + 3] = 0x00;
            }    
        }
    }
}

int main()
{
    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(200, 200), "SFML works!");
    // creating instance of sync_vars
    sync_vars* channel = new sync_vars();

    cpu_chip8* cpu = new cpu_chip8();
    cpu->init_op_table();
    cpu->init_sync_channel(channel);
    std::thread cpu_thread = std::thread(&cpu_chip8::run, cpu); // creating cpu execution thread
    cpu_thread.detach();

    // table translates keyboard key to array index
    std::map<sf::Keyboard::Key, byte> index_table = init_index_table();
    // table translates byte of pixels to real pixels
    std::map<byte, sf::Uint8*> pixel_table = init_pixel_table();

    sf::Uint8* pixels = new sf::Uint8[SCREEN_HEIGHT * SCREEN_WIDTH * 4];    // 4 bytes for every pixel

    sf::Event event;
    while (window->isOpen())
    {
        while(window->pollEvent(event)) {
            switch(event.type) {
                case sf::Event::Closed:
                    window->close();
                break;

                case sf::Event::KeyPressed:
                {
                    std::unique_lock<std::mutex> lock(channel->input_mut);
                    cpu->get_pk_ptr()[index_table[event.key.code]] = 1;
                    channel->key_pressed = true;
                    channel->input_cv.notify_all();
                }
                break;
                
                case sf::Event::KeyReleased:
                {
                    std::unique_lock<std::mutex> lock(channel->input_mut);
                    cpu->get_pk_ptr()[index_table[event.key.code]] = 0;
                }
                break;
            }
        }
    }

    return 0;
}

/*#include <iostream>
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

    my_cpu.init_op_table();
    my_cpu.execute();
    my_cpu.execute();

    return 0;
}*/