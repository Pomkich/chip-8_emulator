#include "constants.h"
#include "cpu.h"
#include "memory.h"
#include <SFML/Graphics.hpp>
#include <thread>

void input_handler(sf::RenderWindow* window_ptr, bool* pressed_keys, sync_vars* channel) {
    // switch keys
    while(true) {
        std::unique_lock<std::mutex> lock(channel->input_mut);
        pressed_keys[0x0] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num0);
        pressed_keys[0x1] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1);
        pressed_keys[0x2] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2);
        pressed_keys[0x3] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3);
        pressed_keys[0x4] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4);
        pressed_keys[0x5] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num5);
        pressed_keys[0x6] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num6);
        pressed_keys[0x7] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num7);
        pressed_keys[0x8] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num8);
        pressed_keys[0x9] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num9);
        pressed_keys[0xA] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
        pressed_keys[0xB] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::B);
        pressed_keys[0xC] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C);
        pressed_keys[0xD] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
        pressed_keys[0xE] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E);
        pressed_keys[0xF] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F);
        channel->input_cv.notify_all(); //
        std::this_thread::sleep_for(std::chrono::milliseconds(32));
    }
}

int main()
{
    
    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(200, 200), "SFML works!");
    cpu_chip8 cpu;
    cpu.init_op_table();
    // creating instance of sync_vars
    sync_vars* channel;
    // creating input thread
    std::thread input_thread = std::thread(input_handler, window, cpu.get_pk_ptr(), channel);
    input_thread.detach();

    while (window->isOpen())
    {
        //cpu.execute();
        // Render();
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