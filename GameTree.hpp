#pragma once
#ifndef _GAME_TREE_
#define _GAME_TREE_
#include <vector>
#include "ColorEnum.hpp"
using namespace std;

class Cell {
public:
    int_fast8_t index = 0, mass = 0;
    Color color;
};
class GameState {
public:
    bool hasAlpha = false, hasBeta = false;
    int_fast8_t level = 0;
    int_fast8_t state = 0; // 1: bad Move, 2: gameOver/player win, 3; gameOver/enemy win
    int_fast16_t alpha = 0, beta = 0;
    Color currentPlayerColor, nextPlayerColor;
    int_fast16_t convertBlue, convertRed;
    vector<Cell> cells;
    inline void swapColor();
    GameState(vector<Cell> _cells, int_fast8_t _s, Color _c, Color _n):
        cells(_cells), state(_s), currentPlayerColor(_c), nextPlayerColor(_n) {};
};
class Student {
public:
    Student();
    void makeMove(int Record[5][6], int Max[5][6], Color color[5][6], Color inputColor);
    int getX();
    int getY();
private:
    int execCount;
    int_fast8_t gameTreeLevel = 4, choiceIndex = -1, rounds = 0;
    int x = 0, y = 0;
    bool inited = false;
    Color playerColor = White, enemyColor = White;
    vector<Cell> cells;
    const int_fast8_t critical[30] {2, 3, 3, 3, 3, 2, 3, 4, 4, 4, 4,
        3, 3, 4, 4, 4, 4, 3, 3, 4, 4, 4, 4, 3, 2, 3, 3, 3, 3, 2};
    const int_fast8_t pmSeq[30] {0, 5, 24, 29, 2, 27, 12, 17, 7, 22, 10, 19, 25, 4, 6,
        23, 26, 3, 18, 11, 1, 28, 8, 21, 13, 16, 9, 20, 14, 15}; // availableMoveseq
    const int_fast8_t OpeningSeq[12] {0, 5, 24, 29, 1, 4, 6, 11, 18, 13, 25, 28};
    const int_fast8_t corners[4] {0, 5, 24, 29};
    const int_fast8_t edges[14] {1, 2, 3, 4, 6, 11, 12, 17, 18, 23, 25, 26, 27, 28};
    const vector<vector<int_fast8_t>> adjIndices {{6, 1}, {7, 0, 2}, {8, 1, 3}, {9, 2, 4},
        {10, 3, 5}, {11, 4}, {0, 12, 7}, {1, 13, 6, 8}, {2, 14, 7, 9}, {3, 15, 8, 10},
        {4, 16, 9, 11}, {5, 17, 10}, {6, 18, 13}, {7, 19, 12, 14}, {8, 20, 13, 15},
        {9, 21, 14, 16}, {10, 22, 15, 17}, {11, 23, 16}, {12, 24, 19}, {13, 25, 18, 20},
        {14, 26, 19, 21}, {15, 27, 20, 22}, {16, 28, 21, 23}, {17, 29, 22}, {18, 25},
        {19, 24, 26}, {20, 25, 27}, {21, 26, 28}, {22, 27, 29}, {23, 28}};

    inline void execute(GameState&, const int_fast8_t rawMoveIndex);
    int_fast16_t maxVal(GameState&);
    int_fast16_t minVal(GameState&);
    inline int_fast16_t scoring(GameState&);
    inline const vector<int_fast8_t> availableMoves(const GameState&) const;
};
#endif /* _GAME_TREE_*/