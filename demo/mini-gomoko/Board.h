#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <iomanip>
#include <sstream>

#include "Common.h"
#include "GameEngine.h"


class Board {
private:
    Role board[BOARD_SIZE][BOARD_SIZE];
    Role currentPlayer;
    int turnCount;
    bool gameOver;
    Role winner;
    GameEngine& engine;
    
public:
    // 构造函数
    Board() : engine(GameEngine::getInstance()) {
        initializeBoard();
    }

    // 游戏控制函数
    void startGame() {
        resetGame();
    }

    void resetGame() {
        initializeBoard();
    }

    bool isGameOver() const {
        return gameOver;
    }

    Role getWinner() const {
        return winner;
    }

    bool makeMove(int x, int y, Role role) {
        if (!isValidMove(x, y) || gameOver) {
            return false;
        }
        
        board[x][y] = role;
        turnCount++;
        
        // 通知游戏引擎
        if (role == Role::HUMAN) {
            engine.humanMove({x, y});
        }
        
        // 检查是否获胜
        if (checkWin(x, y, role)) {
            gameOver = true;
            winner = role;
            return true;
        }
        
        // 检查是否平局
        if (turnCount >= BOARD_SIZE * BOARD_SIZE) {
            gameOver = true;
            winner = Role::NONE;
            return true;
        }
        
        switchPlayer();
        return true;
    }

    bool isValidMove(int x, int y) const {
        return isLegalPoint(x, y) && board[x][y] == Role::NONE;
    }

    // 显示函数
    void display() const {
        std::cout << "\n";
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                std::cout << getRoleSymbol(board[i][j]) << " ";
            }
            std::cout << "\n";
        }
    }

    void displayWithCoordinates() const {
        std::cout << "\n   ";
        for (int j = 0; j < BOARD_SIZE; ++j) {
            std::cout << std::setw(2) << j << " ";
        }
        std::cout << "\n";
        
        for (int i = 0; i < BOARD_SIZE; ++i) {
            std::cout << std::setw(2) << i << " ";
            for (int j = 0; j < BOARD_SIZE; ++j) {
                std::cout << getRoleSymbol(board[i][j]) << "  ";
            }
            std::cout << "\n";
        }
    }

    // 获取信息函数
    Role getRole(int x, int y) const {
        if (isLegalPoint(x, y)) {
            return board[x][y];
        }
        return Role::NONE;
    }

    int getTurnCount() const {
        return turnCount;
    }

    Role getCurrentPlayer() const {
        return currentPlayer;
    }

private:
    // 私有函数实现
    void initializeBoard() {
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                board[i][j] = Role::NONE;
            }
        }
        currentPlayer = Role::HUMAN;
        turnCount = 0;
        gameOver = false;
        winner = Role::NONE;
    }

    bool checkWin(int x, int y, Role role) {
        // 检查四个方向
        for (int dir = 0; dir < 4; ++dir) {
            if (checkDirection(x, y, dx[dir], dy[dir], role)) {
                return true;
            }
        }
        return false;
    }

    bool checkDirection(int x, int y, int dx, int dy, Role role) {
        int count = 1; // 包括当前位置
        
        // 向前检查
        int nx = x + dx, ny = y + dy;
        while (isLegalPoint(nx, ny) && board[nx][ny] == role) {
            count++;
            nx += dx;
            ny += dy;
        }
        
        // 向后检查
        nx = x - dx;
        ny = y - dy;
        while (isLegalPoint(nx, ny) && board[nx][ny] == role) {
            count++;
            nx -= dx;
            ny -= dy;
        }
        
        return count >= 5;
    }

    void switchPlayer() {
        currentPlayer = (currentPlayer == Role::HUMAN) ? Role::COMPUTER : Role::HUMAN;
    }

    char getRoleSymbol(Role role) const {
        switch (role) {
            case Role::HUMAN:
                return 'O';
            case Role::COMPUTER:
                return 'X';
            case Role::NONE:
            default:
                return '.';
        }
    }

    bool isLegalPoint(int x, int y) const {
        return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
    }
};

#endif // BOARD_H