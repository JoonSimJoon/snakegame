#include <ncurses.h>
#include <iostream>
#include "map.hpp"

int main()
{
    initscr();               // Initialize the ncurses screen
    start_color();           // Enable color functionality
    noecho();                // Disable echoing of typed characters
    cbreak();                // Disable line buffering

    // Check if the terminal supports colors
    if (!has_colors()) {
        endwin();
        std::cerr << "Your terminal does not support colors." << std::endl;
        return 1;
    }

    // Define a custom gray color if the terminal supports it
    if (can_change_color()) {
        init_color(COLOR_BLACK, 500, 500, 500); // Define gray (50% intensity for R, G, B)
    }

    // Initialize color pairs
    init_pair(1, COLOR_WHITE, COLOR_BLACK); // Blank with gray background
    init_pair(2, COLOR_BLUE, COLOR_BLACK);  // Wall with gray background
    init_pair(3, COLOR_CYAN, COLOR_BLACK);  // Immutable wall with gray background
    init_pair(4, COLOR_YELLOW, COLOR_BLACK); // Snake's head with gray background
    init_pair(5, COLOR_GREEN, COLOR_BLACK);  // Snake's body with gray background

    // Set the background color of the entire screen to gray
    bkgd(COLOR_PAIR(1));

    // Get the map
    auto map = get_map();
    int rows = map.size();
    int cols = map[0].size();

    // Draw the map as a square
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int cell = map[i][j];
            char ch = ' '; // Default to space for blank cells
            int color = 1;

            switch (cell) {
                case 0: ch = ' '; color = 1; break; // Blank
                case 1: ch = '#'; color = 2; break; // Wall
                case 2: ch = '#'; color = 3; break; // Immutable wall
                case 3: ch = '#'; color = 4; break; // Snake's head
                case 4: ch = '#'; color = 5; break; // Snake's body
                default: ch = ' '; color = 1; break;
            }

            attron(COLOR_PAIR(color));
            // Scale horizontally by doubling the column index
            mvaddch(i, j, ch);       // Left part of the box
            //mvaddch(i, j * 2 + 1, ch);   // Right part of the box
            attroff(COLOR_PAIR(color));
        }
    }

    // Move the pointer to (10, 10)
    move(10, 10);

    // Refresh the screen to display the map
    refresh();

    // Wait for user input before exiting
    getch();

    // Cleanup
    endwin();
    return 0;
}