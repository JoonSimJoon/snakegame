#pragma once
#include <vector>

class Map {
public:
    Map(int width = 23, int height = 23);
    const std::vector<std::vector<int>>& getMapData() const;
    void generate(); // 맵 생성 함수
    int getWidth() const { return width; }
    int getHeight() const { return height; }
private:
    int width, height;
    std::vector<std::vector<int>> mapData;
};
