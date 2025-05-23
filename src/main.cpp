#include <ncurses.h>
#include <iostream>
#include "map.hpp"
#include "game.hpp"
#include "snake.hpp"

int main()
{
    Game game;
    game.run(); // Call the function to generate the screen
    return 0;
}