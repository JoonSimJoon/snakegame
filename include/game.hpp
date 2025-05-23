#pragma once
#include "snake.hpp"
#include "map.hpp"

class Game {
public:
    Game();
    void run();

private:
    void init_screen();         // 화면 초기화
    void update_screen();       // 화면 갱신
    void process_input();       // 사용자 입력 처리
    void update_state();        // 게임 상태 갱신
    bool check_collision();     // 충돌 검사
    //void spawn_item();          // 아이템 생성
    void reset();               // 게임 리셋
    void game_over();           // 게임 오버 처리
    //void show_score();          // 점수 표시
    bool snake_alive() const; // 뱀의 생존 여부 확인
    Snake snake;
    Map map;
    bool is_running;
    int score;
    // 기타 상태 변수
};