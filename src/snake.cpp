#include "snake.hpp"
#include <vector>
#include <utility>

enum Direction { UP = 0, DOWN, LEFT, RIGHT };

Snake::Snake(int x, int y) {
    body.clear();
    body.push_back({y, x}); // 머리 좌표 (행, 열)
    direction = RIGHT;
    length = 1;
    score = 0;
    alive = true;
}

void Snake::set_direction(int dir) {
    // 반대 방향으로는 이동 불가
    if ((direction == UP && dir == DOWN) ||
        (direction == DOWN && dir == UP) ||
        (direction == LEFT && dir == RIGHT) ||
        (direction == RIGHT && dir == LEFT)) {
        return;
    }
    direction = dir;
}

void Snake::move(int dx, int dy) {
    if (!alive) return;
    // 새 머리 좌표 계산
    int new_y = body.front().first + dy;
    int new_x = body.front().second + dx;
    body.insert(body.begin(), {new_y, new_x});
    // 꼬리 삭제 (grow 시에는 삭제하지 않음)
    if ((int)body.size() > length)
        body.pop_back();
}

void Snake::grow() {
    length++;
    score += 10;
    // grow 시 move에서 꼬리를 삭제하지 않으므로 자동으로 길어짐
}

bool Snake::check_collision() {
    // 머리가 몸통과 부딪히는지 검사
    auto head = body.front();
    for (size_t i = 1; i < body.size(); ++i) {
        if (body[i] == head) {
            alive = false;
            return true;
        }
    }
    return false;
}