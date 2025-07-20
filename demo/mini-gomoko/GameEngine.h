#pragma once
#include <utility>
#include <chrono>
#include <random>

#include "Common.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>

using namespace std;

class GameEngine {

public:
    static GameEngine& getInstance() {
        static GameEngine instance;
        return instance;
    }

    pair<int, int>  humanMove(pair<int, int> m) {
        doMove(m, Role::HUMAN);
        return m;
    }
    
    pair<int, int> computerMove(pair<int, int>) {
        using namespace std::chrono;
        
        md = MoveData();
        int CurrentMaxDepth = decideSearchDepth();

        vector<pair<int, int>> moves = genMoves(Role::COMPUTER);


        auto startTime = steady_clock::now();

        for (int depth = 1; depth <= CurrentMaxDepth; ++depth) {
            md.maxDepth = maxDepth = depth;
            md.bestScore = INT_MIN;

            for (auto &mv : moves) {
                auto now = steady_clock::now();
                auto elapsed = duration_cast<milliseconds>(now - startTime).count();
                if (elapsed >= MAX_TIME_MS) {
                    goto TIMEOUT_EXIT;
                }

                doMove(mv, Role::COMPUTER);
                int score = -negaMax(0, -INF, INF, Role::HUMAN);
                undoMove(mv, Role::COMPUTER);

                if (score > md.bestScore) {
                    md.bestScore = score;
                    md.bestMove = mv;
                }
            }
        }

    TIMEOUT_EXIT:

        doMove(md.bestMove, Role::COMPUTER);
        clearHashTable();
        md.summarize();
        
        return md.bestMove;
    }


private:
    struct PointStatus {
        unsigned long long zobristHash[2] = { 0 };
        Role role;
        int neighborCnt {};
        int score {};
    };

    struct HashItem {
        int depth = 0;                 
        int score = INT_MIN;              
        int turn = 0;                    
        pair<int, int> move = {-1, -1};   
        HashItem() = default;
        HashItem(int d, int s, int t) {depth = d, score = s, turn = t;}
    };

    struct MoveData {
        chrono::steady_clock::time_point startTime{chrono::steady_clock::now()};
        
        double evaluateMoveMaxTimeInMs = 0.0;
        double recognizePatternMaxTimeInMs = 0.0;
        double thinkingTimeInMs = 0.0;
        pair<int, int> bestMove = {-1, -1};
        int bestScore = INT_MIN;
        int alphaCutCnt = 0;
        int betaCutCnt = 0;
        int hashHitCnt = 0;
        int searchCnt = 0;
        int maxDepth = 0;

        void summarize() {
            thinkingTimeInMs = chrono::duration<double, milli>(
                chrono::steady_clock::now() - startTime).count();

            double hitRate = (searchCnt > 0) ? (100.0 * hashHitCnt / searchCnt) : 0.0;

            cout 
                << "===[统计信息]===\n"
                << "思考时间(ms): " << thinkingTimeInMs << "\n"
                << "选择着法: [" << bestMove.first << ", " << bestMove.second << "]\n"
                << "着法评分: " << bestScore << '\n' 
                // << "evaluateMove函数最大耗时: " << evaluateMoveMaxTimeInMs << "\n"
                // << "recognizePatter函数最大耗时: " << recognizePatternMaxTimeInMs << '\n'
                << "总搜索次数: " << searchCnt << "\n"
                << "最大搜索深度: " << maxDepth << '\n'
                << "缓存命中数: " << hashHitCnt << "\n"
                << "缓存命中率: " << fixed << setprecision(2) << hitRate << " %\n"
                // << "发生 Alpha 截断数: " << alphaCutCnt << "\n"
                << "发生 Beta 截断数: " << betaCutCnt << "\n";
        }
    };


    unsigned long long zobristHash = 0ULL;
    const int INF = 1e9;
    const int MAX_DEPTH = 20;
    const int MAX_TIME_MS = 5000;
    int turn = 0;
    int maxDepth = 1;
    unordered_map<unsigned long long, HashItem> hashTable;
    PointStatus board[BOARD_SIZE][BOARD_SIZE];
    MoveData md;

    int decideSearchDepth() {
        int stones = turn;

        if (stones <= 4) return 2;              // 开局，快速启发判断
        else if (stones <= 8) return 4;         // 初期
        else if (stones <= 14) return 6;        // 中期
        else if (stones <= 22) return 8;        // 中后期
        else return MAX_DEPTH;              // 终局力争精准终结
    }

    int negaMax(int depth, int alpha, int beta, Role role) {
        md.searchCnt++;
        auto it = hashTable.find(zobristHash);
        if (it != hashTable.end()) {
            md.hashHitCnt++;
            return it->second.score;
        }
        hashTable[zobristHash] = HashItem(depth, INT_MIN, turn);

        if (depth == maxDepth) {
            hashTable[zobristHash].score = evaluate(role);
            return hashTable[zobristHash].score;
        }

        vector<pair<int, int>> moves = genMoves(role);

        for (auto &move : moves) {
            doMove(move, role);

            int score = -negaMax(depth + 1, -beta, -alpha, (Role)(role ^ 1));

            undoMove(move, role);
            
            if (score > hashTable[zobristHash].score) {
                hashTable[zobristHash].score = score;
                hashTable[zobristHash].move = move;
            }

            alpha = max(alpha, score);

            if (alpha >= beta) {
                md.betaCutCnt++;
                return hashTable[zobristHash].score;
            }
        }
        return hashTable[zobristHash].score;
    }

    int evaluate(Role role) {


        long long scoreCurr = 0;
        long long scoreEnemy = 0;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (board[i][j].role > -1) {
                    if (board[i][j].role == role) scoreCurr += evaluateMove({i, j}, board[i][j].role);
                    else scoreEnemy += evaluateMove({i, j}, board[i][j].role);
                }
            }
        }


        return scoreCurr - scoreEnemy;
    }

    int evaluateMove(pair<int, int> m, Role role) {
        // chrono::steady_clock::time_point evaluateMoveStartTime = chrono::steady_clock::now();

        int totalScore = 0;
        int x = m.first;
        int y = m.second;

        for (int dir = 0; dir < 4; ++dir) {
            totalScore += recognizePattern(getLinePattern(x, y, dx[dir], dy[dir], role));
        }

        // md.evaluateMoveMaxTimeInMs = max(md.evaluateMoveMaxTimeInMs, chrono::duration<double, milli>(chrono::steady_clock::now() - evaluateMoveStartTime).count());
        return totalScore;
    }


    int evaluatePoint(pair<int, int> p) {
        return board[p.first][p.second].neighborCnt;
    }

    int evaluatePoint(pair<int, int> p, Role r) {
        return evaluateMove(p, r);
    }

    vector<pair<int, int>> genMoves(Role r) {
        vector<pair<int ,int>> moves;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (board[i][j].role == Role::NONE && board[i][j].neighborCnt > 0) {
                    moves.emplace_back(i, j);
                    board[i][j].score = evaluatePoint({i, j}, r);
                }
            }
        }

        sort(moves.begin(), moves.end(), [&](const pair<int, int>& a, const pair<int, int>& b) {
            return board[a.first][a.second].score > board[b.first][b.second].score;
        });

        return moves;
    }

    void doMove(pair<int, int> m, Role r) {
        int R = 3;
        turn++;
        zobristHash ^= board[m.first][m.second].zobristHash[(int)r];
        board[m.first][m.second].role = r;
        for (int i = m.first - R; i <= m.first + R; ++i) {
            for (int j = m.second - R; j <= m.second + R; ++j) {
                if (isLegalPoint(i, j)) {
                    board[i][j].neighborCnt++;
                }
            }
        }
    }

    void undoMove(pair<int, int> m, Role r) {
        int R = 3;
        turn--;
        zobristHash ^= board[m.first][m.second].zobristHash[(int)r];
        board[m.first][m.second].role = Role::NONE;
        for (int i = m.first - R; i <= m.first + R; ++i) {
            for (int j = m.second - R; j <= m.second + R; ++j) {
                if (isLegalPoint(i, j)) {
                    board[i][j].neighborCnt--;
                }
            }
        }
    }
    
    string getLinePattern(int x, int y, int dx, int dy, Role role) {
        string pattern;
        for (int i = -4; i <= 4; ++i) {
            int nx = x + i * dx;
            int ny = y + i * dy;

            if (!isLegalPoint(nx, ny)) {
                pattern += 'B'; // 边界视为阻挡
            } else if (nx == x && ny == y) {
                pattern += '1'; // 假设当前点落子
            } else {
                Role r = board[nx][ny].role;
                if (r == Role::NONE) pattern += '0';
                else if (r == role) pattern += '1';
                else pattern += '2';
            }
        }
        return pattern;
    }

    int recognizePattern(const string& s) {
        // chrono::steady_clock::time_point recognizePatternStartTime = chrono::steady_clock::now();
        int score = 0;
        int count[PATTERN_TYPE_COUNT] = {0};

        // 检测各类模式出现次数
        if (s.find("11111") != string::npos) count[FIVE]++;
        
        // 活四
        for (const string& pat : {"011110"}) {
            size_t pos = s.find(pat);
            while (pos != string::npos) {
                count[FOUR]++;
                pos = s.find(pat, pos + 1);
            }
        }

        // 冲四
        for (const string& pat : {"011112", "211110", "10111", "11011", "11101"}) {
            size_t pos = s.find(pat);
            while (pos != string::npos) {
                count[BLOCKED_FOUR]++;
                pos = s.find(pat, pos + 1);
            }
        }

        // 活三
        for (const string& pat : {"01110", "010110", "011010"}) {
            size_t pos = s.find(pat);
            while (pos != string::npos) {
                count[THREE]++;
                pos = s.find(pat, pos + 1);
            }
        }

        // 眠三
        for (const string& pat : {"001112", "211100", "021110", "011012"}) {
            size_t pos = s.find(pat);
            while (pos != string::npos) {
                count[BLOCKED_THREE]++;
                pos = s.find(pat, pos + 1);
            }
        }

        // 活二
        for (const string& pat : {"00110", "01010", "01100", "00110"}) {
            size_t pos = s.find(pat);
            while (pos != string::npos) {
                count[TWO]++;
                pos = s.find(pat, pos + 1);
            }
        }

        // 眠二
        for (const string& pat : {"000112", "211000", "021100", "001102"}) {
            size_t pos = s.find(pat);
            while (pos != string::npos) {
                count[BLOCKED_TWO]++;
                pos = s.find(pat, pos + 1);
            }
        }

        // 组合所有得分
        for (int i = 0; i < PATTERN_TYPE_COUNT; ++i) {
            score += count[i] * patternScore[i];
        }
        // md.recognizePatternMaxTimeInMs = max(md.recognizePatternMaxTimeInMs, chrono::duration<double, milli>(chrono::steady_clock::now() - recognizePatternStartTime).count());
        return score;
    }

    void clearHashTable() {
        int cnt = 0;
        auto iter = hashTable.begin();
        while (iter != hashTable.end()) {
            if (iter->second.turn + iter-> second.depth < turn) {
                cnt++;
                iter = hashTable.erase(iter);  // 返回下一个合法迭代器
            } else {
                ++iter;
            }
        }
        LOG(INFO, "已经清理 [" << cnt <<"] 个缓存记录 ");
    }


    GameEngine() {
        mt19937_64 rng(random_device{}());
        uniform_int_distribution<uint64_t> dist;

        zobristHash = dist(rng);
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                board[i][j].role = Role::NONE;
                board[i][j].zobristHash[0] = dist(rng);
                board[i][j].zobristHash[1] = dist(rng);
            }
        }
    };

    ~GameEngine() = default;

    GameEngine(const GameEngine&) = delete;

    GameEngine& operator=(const GameEngine&) = delete;

};