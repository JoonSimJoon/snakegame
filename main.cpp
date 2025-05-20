#include <ncurses.h>
#include <iostream>
#include "map.hpp"

int main()
{
    initscr();
    start_color();
    // use_default_colors(); // 필요시 주석 해제

    // 색상쌍: 글자색, 배경색(터미널 기본 배경)
    init_pair(1, COLOR_WHITE, -1);   // 빈칸
    init_pair(2, COLOR_BLUE, -1);    // 벽
    init_pair(3, COLOR_CYAN, -1);    // 이뮤너블 벽
    init_pair(4, COLOR_YELLOW, -1);  // 머리
    init_pair(5, COLOR_GREEN, -1);   // 몸통

    auto map = get_map();
    int rows = map.size();
    int cols = map[0].size();

    WINDOW *win = newwin(rows + 2, cols + 2, 0, 0);
    wbkgd(win, COLOR_PAIR(1));
    box(win, 0, 0);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int cell = map[i][j];
            chtype ch;
            int color = 1;
            switch(cell) {
                case 0: ch = ' '; color = 1; break; // 빈칸
                case 1: ch = '#'; color = 2; break; // 벽
                case 2: ch = '@'; color = 3; break; // 이뮤너블 벽
                case 3: ch = 'O'; color = 4; break; // 머리
                case 4: ch = 'o'; color = 5; break; // 몸통
                default: ch = '?'; color = 1; break;
            }
            wattron(win, COLOR_PAIR(color));
            mvwaddch(win, i + 1, j + 1, ch);
            wattroff(win, COLOR_PAIR(color));
        }
    }

    wrefresh(win);
    getch();
    delwin(win);
    endwin();
    return 0;
}