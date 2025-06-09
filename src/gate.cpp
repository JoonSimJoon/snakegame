#include "gate.hpp"
#include <cstdlib>
#include <ctime>
#include <algorithm>

const int dx[4] = {-1, 1, 0, 0};
const int dy[4] = {0, 0, -1, 1};


// 맵에서 벽(1) 위치 중 임의의 두 곳을 골라 게이트로 지정
void Gate::spawn( std::vector<std::vector<int>>& map) {
    wall_positions.clear();
    int rows = map.size();
    int cols = map[0].size();

    // 벽(1) 위치 수집
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (map[i][j] == 1) {
                wall_positions.push_back({i, j});
            }
        }
    }

    if (wall_positions.size() < 2) return; // 벽이 2개 미만이면 게이트 생성 불가

    // 랜덤으로 두 개 선택 (겹치지 않게)
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    int idx1 = std::rand() % wall_positions.size();
    int idx2;
    do {
        idx2 = std::rand() % wall_positions.size();
    } while (idx2 == idx1);

    gate1 = wall_positions[idx1];
    gate2 = wall_positions[idx2];
    exists = true;
    map[gate1.first][gate1.second] = 7; // 게이트1
    map[gate2.first][gate2.second] = 7; // 게이트2

}

std::pair<int, int> Gate::get_entry() const {
    return gate1;
}

std::pair<int, int> Gate::get_exit() const {
    return gate2;
}

bool Gate::is_gate(int x, int y) const {
    return (exists && ((gate1.first == x && gate1.second == y) || (gate2.first == x && gate2.second == y)));
}


// 게이트 진입 시 출구 앞 좌표와 방향 반환
std::pair<std::pair<int, int>, int> Gate::teleport_with_dir(
    int in_x, int in_y, int in_dir, const std::vector<std::vector<int>>& map) const
{
    // 출구 게이트 좌표 결정
    std::pair<int, int> out_gate;
    if (gate1.first == in_x && gate1.second == in_y)
        out_gate = gate2;
    else
        out_gate = gate1;

    // 출구에서 나올 수 있는 방향 찾기 (벽이 아닌 곳)
    for (int dir = 0; dir < 4; ++dir) {
        int nx = out_gate.first + dx[dir];
        int ny = out_gate.second + dy[dir];
        // 맵 범위 내 & 벽(1)이나 게이트(7)가 아닌 곳
        if (nx >= 0 && nx < (int)map.size() && ny >= 0 && ny < (int)map[0].size()) {
            int cell = map[nx][ny];
            if (cell != 1 && cell != 7) {
                // 출구 앞 좌표와 방향 반환
                return {{nx, ny}, dir};
            }
        }
    }
    // 만약 출구가 막혀있으면, 그냥 게이트 좌표와 기존 방향 반환
    return {out_gate, in_dir};
}

std::pair<int, int> Gate::teleport(int x, int y) const {
    if (gate1.first == x && gate1.second == y)
        return gate2;
    else if (gate2.first == x && gate2.second == y)
        return gate1;
    else
        return {x, y}; // 게이트가 아니면 그대로 반환
}