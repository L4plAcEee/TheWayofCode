#pragma once

#define LOG(Level, S) std::cout << "[调试信息] : [" << #Level << "] : " << S << "  " << std::endl

#define BOARD_SIZE (int)(15)

enum PatternType {
    FIVE = 0,
    FOUR,
    BLOCKED_FOUR,
    THREE,
    BLOCKED_THREE,
    TWO,
    BLOCKED_TWO,
    PATTERN_TYPE_COUNT // 自动获取枚举数量
};

const int patternScore[PATTERN_TYPE_COUNT] = {
    100000,   // FIVE（必胜）
    10000,    // FOUR（活四）
    5000,     // BLOCKED_FOUR（冲四）
    1000,     // THREE（活三）
    500,      // BLOCKED_THREE（眠三）
    200,      // TWO（活二）
    100       // BLOCKED_TWO（眠二）
};


enum Role {
    NONE = -1,
    HUMAN,
    COMPUTER
};

// 四个方向：横、竖、主对角线、副对角线
const int dx[4] = {1, 0, 1, 1};
const int dy[4] = {0, 1, 1, -1};

bool isLegalPoint (int x, int y) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}