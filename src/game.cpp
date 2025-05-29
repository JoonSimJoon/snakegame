#include "game.hpp"
#include <iostream>
#include <ncurses.h>

Map map; // Declare a global map object
Snake snake; // Declare a global snake object
enum Direction { UP = 0, DOWN, LEFT, RIGHT };


Game::Game()
    : snake(), map(), is_running(false), score(0)
{
    // 필요시 추가 초기화 코드 작성
}


void Game::init_screen() {
    initscr();               // ncurses 화면 초기화
    start_color();           // 컬러 기능 활성화
    noecho();                // 입력 문자 화면에 표시 안 함
    cbreak();                // 라인 버퍼링 해제

    // 터미널이 컬러 지원하는지 확인
    if (!has_colors()) {
        endwin();
        std::cerr << "Your terminal does not support colors." << std::endl;
        exit(1);
    }

    // 터미널이 컬러 변경 지원 시, 회색 정의
    if (can_change_color()) {
        init_color(COLOR_BLACK, 500, 500, 500); // 회색 (R,G,B 50%)
    }

    // 컬러 페어 초기화
    init_pair(1, COLOR_WHITE, COLOR_BLACK); // 빈 칸
    init_pair(2, COLOR_BLUE, COLOR_BLACK);  // 벽
    init_pair(3, COLOR_CYAN, COLOR_BLACK);  // 고정 벽
    init_pair(4, COLOR_YELLOW, COLOR_BLACK); // 뱀 머리
    init_pair(5, COLOR_GREEN, COLOR_BLACK);  // 뱀 몸통

    // 전체 배경색 지정
    bkgd(COLOR_PAIR(1));
}

void Game::update_screen() {
    clear(); // 화면 초기화

    // 맵 정보 가져오기
    auto mapData = map.getMapData();
    int rows = map.getHeight();
    int cols = map.getWidth();

    // 맵 출력
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int cell = mapData[i][j];
            char ch = ' ';
            int color = 1;

            switch (cell) {
                case 0: ch = ' '; color = 1; break; // 빈 칸
                case 1: ch = '#'; color = 2; break; // 벽
                case 2: ch = '#'; color = 3; break; // 고정 벽
                case 3: ch = '#'; color = 4; break; // 뱀 머리
                case 4: ch = '#'; color = 5; break; // 뱀 몸통
                default: ch = ' '; color = 1; break;
            }

            attron(COLOR_PAIR(color));
            mvaddch(i, j * 2, ch);
            mvaddch(i, j * 2 + 1, ' ');
            attroff(COLOR_PAIR(color));
        }
    }

    refresh(); // 화면 갱신
}


void Game::process_input() {
    int ch = getch();
    switch (std::tolower(ch)) {
        case 'w':
            snake.set_direction(UP);
            break;
        case 'a':
            snake.set_direction(LEFT);
            break;
        case 's':
            snake.set_direction(DOWN);
            break;
        case 'd':
            snake.set_direction(RIGHT);
            break;
        case 'q':
            is_running = false; // 게임 종료
            Quit = true;
            break;
        default:
            break;
    }
}

void Game::update_state() {
    // 현재 방향에 따라 dx, dy 결정
    int dx = 0, dy = 0;
    switch (snake.get_direction()) {
        case UP:    dx = -1; break;
        case DOWN:  dx = 1;  break;
        case LEFT:  dy = -1; break;
        case RIGHT: dy = 1;  break;
    }

    // 이전 뱀 위치를 맵에서 지움 (몸통, 머리 모두)
    for (const auto& part : snake.get_body()) {
        int x = part.first;
        int y = part.second;
        if (x >= 0 && x < map.getHeight() && y >= 0 && y < map.getWidth()) {
            if (map.getMapData()[x][y] == 3 || map.getMapData()[x][y] == 4)
                map.getMapData()[x][y] = 0; // 빈 칸으로
        }
    }

    // 뱀 이동
    snake.move(dx, dy);

    // 벽 충돌 검사
    auto head = snake.get_head();
    if (head.first < 0 || head.first >= map.getHeight() ||
        head.second < 0 || head.second >= map.getWidth() ||
        map.getMapData()[head.first][head.second] == 1) // 1: 벽
    {
        is_running = false;
        snake.set_alive(false);
        return;
    }

    // 자기 몸과의 충돌 검사
    if (snake.check_collision()) {
        is_running = false;
        return;
    }

    // 열매(아이템) 먹었는지 검사 (예시: 5번 셀에 열매가 있다고 가정)
    if (map.getMapData()[head.first][head.second] == 5) {
        snake.grow();
        // 아이템 재배치 등 추가 로직 필요
    }

    // 뱀의 새로운 위치를 맵에 표시 (머리: 3, 몸통: 4)
    const auto& body = snake.get_body();
    for (size_t i = 0; i < body.size(); ++i) {
        int x= body[i].first;
        int y = body[i].second;
        if (x >= 0 && x < map.getHeight() && y >= 0 && y < map.getWidth()) {
            if (i == 0)
                map.getMapData()[x][y] = 3; // 머리
            else
                map.getMapData()[x][y] = 4; // 몸통
        }
    }
}

bool Game::check_collision() {
    auto head = snake.get_head();

    // 벽 충돌 검사
    if (head.first < 0 || head.first >= map.getHeight() ||
        head.second < 0 || head.second >= map.getWidth() ||
        map.getMapData()[head.first][head.second] == 1) // 1: 벽
    {
        snake.set_alive(false);
        return true;
    }

    // 자기 몸과의 충돌 검사
    if (snake.check_collision()) {
        snake.set_alive(false);
        return true;
    }

    return false;
}

void Game::reset() {
    map.generate();
    // int x = map.getWidth() / 2;
    // int y = map.getHeight() / 2;
    // snake = Snake(x, y); // (x, y) 좌표계로 중앙에 뱀 생성
    // snake.set_direction(LEFT); // 시작 방향을 LEFT로 설정
    snake = Snake(map);
    is_running = true;
    score = 0;
}




void Game::game_over() {
    clear();
    mvprintw(map.getHeight() / 2, (map.getWidth() - 9) , "GAME OVER");
    mvprintw(map.getHeight() / 2 + 1, (map.getWidth() - 15), "Press any key to exit...");
    refresh();
    getch(); // 아무 키나 누를 때까지 대기
    endwin(); // ncurses 종료
    if(Quit) {
        std::cout<<"강제종료\n";
    }
    std::cout << "최종 점수: " << score << std::endl;
}

bool Game::snake_alive() const {
    return snake.get_alive();
}
void Game::run() {
    reset();
    init_screen();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    is_running = true;
    while (is_running) {
        process_input();
        update_state();
        update_screen();

        // 디버깅: snake_alive 상태와 뱀 머리 좌표, alive 상태 출력
        //auto head = snake.get_head();
        //mvprintw(0, 0, "snake_alive: %d, head: (%d, %d), alive: %d   ",
        //         snake_alive(), head.first, head.second, snake.get_alive());
        refresh();

        if (!snake_alive()) {
            is_running = false;
            break;
        }

        napms(300);
    }
    game_over();
}
// snake_alive()는 snake가 살아있는지 반환하는 함수로, 아래처럼 구현할 수 있습니다.
// bool Game::snake_alive() const { return snake.is_alive(); }