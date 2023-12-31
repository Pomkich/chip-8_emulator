#include "constants.h"
#include "cpu.h"
#include "memory.h"
#include <SFML/Graphics.hpp>
#include <thread>
#include <memory>

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

    return index_table;
}

// it's slow to check every pixel every tick, so i want to construct
// data structure from which i can take fragments of 8 pixels 
void init_pixel_table(std::map<byte, sf::Uint8*>& res) {
    byte array_id = 0;
    // for every bit mask need to construct data, that corresponds to real pixels
    for (int bit_mask8 = 0; bit_mask8 < 256; bit_mask8++) { // 2^8 = 256
        res[bit_mask8] = new sf::Uint8[32]; // every pixel stored as 4 bytes -> 8 * 4 = 32
        for (int pixel_id = 0; pixel_id < 8; pixel_id++) {
            array_id = (7 - pixel_id);  // need to translate original pixel to place in array
            if ((bit_mask8 >> pixel_id) & 0b00000001) { // white pixel
                res[bit_mask8][array_id * 4 + 0] = 0xFF;
                res[bit_mask8][array_id * 4 + 1] = 0xFF;
                res[bit_mask8][array_id * 4 + 2] = 0xFF;
                res[bit_mask8][array_id * 4 + 3] = 0xFF;
            }
            else {  //black pixel
                res[bit_mask8][array_id * 4 + 0] = 0x00;
                res[bit_mask8][array_id * 4 + 1] = 0x00;
                res[bit_mask8][array_id * 4 + 2] = 0x00;
                res[bit_mask8][array_id * 4 + 3] = 0xFF;
            }    
        }
    }
}

int main()
{
    std::unique_ptr<sf::RenderWindow> window = std::make_unique<sf::RenderWindow>();
    window->create(sf::VideoMode(SCREEN_WIDTH * X_SCALE, SCREEN_HEIGHT * Y_SCALE), "chip-8 emulator");
    
    // creating instance of sync_vars
    std::shared_ptr<sync_vars> channel = std::make_shared<sync_vars>();
    channel->key_code = -1;
    channel->key_pressed = false;
    channel->wait_for_key = false;
    channel->reg_to_store = -1;

    std::shared_ptr<cpu_chip8> cpu = std::make_shared<cpu_chip8>();
    cpu->init_op_table();
    cpu->init_sync_channel(channel);

    // table translates keyboard key to array index
    std::map<sf::Keyboard::Key, byte> index_table = init_index_table();

    // table translates byte of pixels to real pixels
    std::map<byte, sf::Uint8*> pixel_table;
    init_pixel_table(pixel_table);

    sf::Uint8* pixels = new sf::Uint8[SCREEN_HEIGHT * SCREEN_WIDTH * 4];    // 4 bytes for every pixel
    sf::Texture texture;
    texture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
    texture.update(pixels);
    sf::Sprite window_view;
    window_view.setPosition(sf::Vector2f(0, 0));
    window_view.setScale(sf::Vector2f(X_SCALE, Y_SCALE));
    window_view.setTexture(texture);
    
    sf::Clock clock;
    sf::Time elapsed;
    sf::Event event;
    byte bit_mask = 0;
    cpu->reset();
    clock.restart();
    while (window->isOpen())
    {
        // update emulator state
        elapsed = clock.restart();
        cpu->update_to_execute(32);
        cpu->update_timers(elapsed.asMilliseconds() / 16);  // 60 Hz
        cpu->run();
        // read input
        while(window->pollEvent(event)) {
            switch(event.type) {
                case sf::Event::Closed:
                    window->close();
                break;

                case sf::Event::KeyPressed:
                {
                    if (event.key.code == sf::Keyboard::R) { cpu->reset(); }
                    cpu->get_pk_ptr()[index_table[event.key.code]] = 1;
                    channel->key_pressed = false;   // key executed on release, so there false
                }
                break;
                
                case sf::Event::KeyReleased:
                {
                    cpu->get_pk_ptr()[index_table[event.key.code]] = 0;
                    if (channel->wait_for_key) {    // if waits key
                        channel->key_code = index_table[event.key.code];    // store pressed key
                        channel->key_pressed = true;    // tell that key was pressed
                        channel->wait_for_key = false;
                        cpu->notify_key_pressed();
                    }
                }
                break;
            }
        }
        // Render
        window->clear(sf::Color::Black);
        // update pixel array
        for (int bits_chunk8 = 0; bits_chunk8 < (SCREEN_WIDTH / 8) * SCREEN_HEIGHT; bits_chunk8++) {
            for (int pixel_chunk32 = 0; pixel_chunk32 < 32; pixel_chunk32++) {
                bit_mask = cpu->get_mem_ptr()->read(DISPLAY_START_AREA + bits_chunk8);
                pixels[pixel_chunk32 + (32 * bits_chunk8)] = pixel_table[bit_mask][pixel_chunk32];
            }
        }
        texture.update(pixels);
        window->draw(window_view);

        window->display();
        // 60 times per second
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}