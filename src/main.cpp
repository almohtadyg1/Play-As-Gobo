#include "Game.hpp"
#include <iostream>
#include <exception>

int main() {
    try {
        PlayAsGobo::Game game;
        
        if (!game.IsInitialized()) {
            std::cerr << "Failed to initialize game. Exiting..." << std::endl;
            return -1;
        }
        
        game.Run();
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return -1;
    }
}