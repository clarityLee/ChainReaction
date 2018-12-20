#pragma once
#ifndef _GAME_TREE_
#define _GAME_TREE_
#include <iostream>
#include <vector>
#include <memory>
#include <iostream>
#include "ColorEnum.hpp"
using namespace std;

class Cell {
public:
    short index = 0, i = 0, j = 0, 
        critical = 0 , mass = 0; // criticalMass, currentMass
    Color color;

    Cell(short _index, short _i, short _j,
        short _mass, short _critical, Color _color):
        index(_index), i(_i), j(_j),
        mass(_mass), critical(_critical), color(_color) {};
};

class State {
public:
    bool hasAlpha = false, hasBeta = false, isBadMove = false, gameOver = false;
    short choiceIndex = -1, treeLevel = -1;
    int score = 0, minmax = 0, alpha = 0, beta = 0;
    Color currentPlayerColor, nextPlayerColor;
    std::vector<Cell> cells;
    
    State() {};
    State(std::vector<Cell> _cells, int _treeLevel,
        Color _currentPlayerColor, Color _nextPlayerColor):
        cells(_cells), treeLevel(_treeLevel),
        currentPlayerColor(_currentPlayerColor), nextPlayerColor(_nextPlayerColor) {};
};

class Student {
public:
    void makeMove(int Record[5][6], int Max[5][6], Color color[5][6], Color inputColor);
    int getX();
    int getY();
private:
    int x = 0, y = 0;
    int gameTreeLevel = 5;
    int rounds = 0;
    bool inited = false;
    Color playerColor;
    Color enemyColor;
    std::vector<Cell> cells;
    int pmSeq[30] {0, 5, 24, 29, 2, 27, 12, 17, 7, 22, 10, 19, 25, 4, 6,
        23, 26, 3, 18, 11, 1, 28, 8, 21, 13, 16, 9, 20, 14, 15}; // possibleMoveSeq
    int OpeningSeq[12] {0, 5, 24, 29, 1, 4, 6, 11, 18, 13, 25, 28};
    std::vector<std::vector<short>> adjIndices;

    void init(int masses[5][6], int critical[5][6], Color color[5][6], Color inputColor);
    void update(int masses[5][6], Color color[5][6], Color inputColor);
    inline short calcIndex(const short i , const short j) const;
    int maxVal(shared_ptr<State>, const int alpha, const int beta,
        const bool hasAlpha, const bool hasBeta);
    int minVal(shared_ptr<State>, const int alpha, const int beta,
        const bool hasAlpha, const bool hasBeta);
    vector<short> rawPossibleMoves(const shared_ptr<State>);
    void nextMove(shared_ptr<State>, const short rawMoveIndex);
    void scoring(shared_ptr<State>, const bool exploded, const bool convertEnemy);
};

#endif /* _GAME_TREE_*/