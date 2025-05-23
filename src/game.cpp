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
            break;
        default:
            break;
    }
}

void Game::update_state() {
    // 현재 방향에 따라 dx, dy 결정
    int dx = 0, dy = 0;
    switch (snake.get_direction()) {
        case UP:    dy = -1; break;
        case DOWN:  dy = 1;  break;
        case LEFT:  dx = -1; break;
        case RIGHT: dx = 1;  break;
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
    // 맵과 뱀을 초기 상태로 재설정
    map.generate(); // 맵 재생성 (맵 크기 등은 필요시 생성자에서 관리)
    snake = Snake(map.getWidth() / 2, map.getHeight() / 2); // 중앙에서 시작
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
    std::cout << "최종 점수: " << score << std::endl;
}

bool Game::snake_alive() const {
    return snake.get_alive();
}

void Game::run() {
    reset();            // 맵, 뱀, 점수 등 초기화
    init_screen();      // ncurses 및 화면 초기화
    nodelay(stdscr, TRUE); // getch가 블로킹되지 않도록 설정
    keypad(stdscr, TRUE);  // 방향키 입력 허용

    is_running = true;
    while (is_running) {
        process_input();    // 사용자 입력 처리
        update_state();     // 게임 상태 갱신
        update_screen();    // 화면 갱신

        // 충돌 시 게임 종료
        if (!snake_alive()) {
            is_running = false;
            break;
        }

        napms(100); // 게임 속도 조절 (100ms 대기)
    }
    game_over(); // 게임 오버 화면 및 종료 처리
}

// snake_alive()는 snake가 살아있는지 반환하는 함수로, 아래처럼 구현할 수 있습니다.
// bool Game::snake_alive() const { return snake.is_alive(); }