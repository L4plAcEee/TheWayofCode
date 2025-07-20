#include <iostream>
#include <string>
#include "Common.h"
#include "Board.h"
#include "GameEngine.h"

using namespace std;

void printWelcome() {
    cout << "========================================\n";
    cout << "           五子棋游戏\n";
    cout << "========================================\n";
    cout << "游戏规则：\n";
    cout << "- 人类玩家使用 ○，电脑使用 ●\n";
    cout << "- 输入坐标格式：x y (例如: 7 7)\n";
    cout << "- 输入 'q' 或 'quit' 退出游戏\n";
    cout << "- 输入 'r' 或 'reset' 重新开始\n";
    cout << "========================================\n\n";
}

void printGameResult(Role winner) {
    cout << "\n========================================\n";
    if (winner == Role::HUMAN) {
        cout << "🎉 恭喜！你赢了！\n";
    } else if (winner == Role::COMPUTER) {
        cout << "💻 电脑获胜！再接再厉！\n";
    } else {
        cout << "🤝 平局！\n";
    }
    cout << "========================================\n";
}

bool handleUserInput(Board& board, const string& input) {
    if (input == "q" || input == "quit") {
        cout << "感谢游戏！再见！\n";
        return false;
    }
    
    if (input == "r" || input == "reset") {
        board.resetGame();
        cout << "游戏已重置！\n";
        return true;
    }
    
    // 解析坐标输入
    istringstream iss(input);
    int x, y;
    if (!(iss >> x >> y)) {
        cout << "❌ 输入格式错误！请输入两个数字，例如：7 7\n";
        return true;
    }
    
    if (!board.isValidMove(x, y)) {
        cout << "❌ 无效移动！请选择一个空位置。\n";
        return true;
    }
    
    // 执行人类移动
    if (board.makeMove(x, y, Role::HUMAN)) {
        cout << "✅ 你的移动：(" << x << ", " << y << ")\n";
        
        if (board.isGameOver()) {
            return true;
        }
        
        cout << "\n💭 电脑思考中...\n";
        
        // 获取电脑移动
        GameEngine& engine = GameEngine::getInstance();
        pair<int, int> computerMove = engine.computerMove({-1, -1});
        
        if (board.makeMove(computerMove.first, computerMove.second, Role::COMPUTER)) {
            cout << "🤖 电脑移动：(" << computerMove.first << ", " << computerMove.second << ")\n";
        }
    }
    
    return true;
}

int main() {
    Board board;
    string input;
    
    printWelcome();
    
    board.startGame();
    
    while (true) {
        cout << "\n";
        board.displayWithCoordinates();
        
        if (board.isGameOver()) {
            printGameResult(board.getWinner());
            
            cout << "\n是否再来一局？(y/n): ";
            getline(cin, input);
            if (input == "y" || input == "Y" || input == "yes") {
                board.resetGame();
                continue;
            } else {
                break;
            }
        }
        
        cout << "\n当前玩家: " << (board.getCurrentPlayer() == Role::HUMAN ? "人类 (○)" : "电脑 (●)") << "\n";
        cout << "请输入移动坐标 (x y) 或命令 (q/quit/r/reset): ";
        
        getline(cin, input);
        
        if (!handleUserInput(board, input)) {
            break;
        }
    }
    
    return 0;
}