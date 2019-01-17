#include <chrono>
#include <queue>
#include <vector>
#include <iostream>
#include "GameTree.hpp"
using namespace std;

inline void GameState::swapColor() {
    const Color _tmp = currentPlayerColor; 
    currentPlayerColor = nextPlayerColor;
    nextPlayerColor = _tmp;
}

Student::Student(): cells(30) {
    for (int_fast8_t i = 0 ; i < 30 ; ++i) cells[i].index = i;
};

void Student::makeMove(int Record[5][6], int Max[5][6], Color color[5][6], Color inputColor) {
    if (playerColor == White) {
        playerColor = inputColor; enemyColor = (inputColor == Blue) ? Red : Blue;
    }
    for (int_fast8_t i = 0, index = 0; i < 5 ; ++i) for (int_fast8_t j = 0 ; j < 6 ; ++j, ++index) {
        cells[index].mass = Record[i][j];
        cells[index].color = color[i][j];
    }
    switch(++rounds) {
        // case 12: gameTreeLevel =  6; break;
        // case  6: gameTreeLevel =  7; break;
        // case 16: gameTreeLevel =  8; break;
        // case 18: gameTreeLevel =  9; break;
        case  8: gameTreeLevel =  5; break;
        case 15: gameTreeLevel =  6; break;
        case 20: gameTreeLevel =  7; break;
    }
    // rounds = 23;
    // gameTreeLevel = 8;
    constexpr auto &&now = std::chrono::high_resolution_clock::now;
    auto start = now();
    cout << " -- Round " << (short) rounds << "...";
    execCount = 0;
    choiceIndex = -1;
    GameState root(cells, 0, playerColor, enemyColor);
    maxVal(root);
    cout << ", choiceIndex: " << (int) choiceIndex;
    // cout << ", minmanx: " << value;
    if (choiceIndex < 0) {
        auto moves = availableMoves(root);
        if (moves.empty()) {x = 0; y = 0;}
        else choiceIndex = moves[0];
        cout << ", choiceIndex: " << (int) choiceIndex;
    }
    
    x = choiceIndex/6; y = choiceIndex%6;

    chrono::duration<double> elapsed = now() - start;
    cout << " GameTree Complete in: " << (int) (elapsed.count() * 1000) << " ms, "
        << "execCount: " << execCount << endl;
};
int Student::getX() {return x;};
int Student::getY() {return y;};

int_fast16_t Student::maxVal(GameState& s) {
    if (s.level == gameTreeLevel || s.state >=2) {return scoring(s);}
    int_fast16_t minimax = -32767;
    const auto moves = availableMoves(s);
    for (int_fast8_t i = 0, size = moves.size() ; i != size ; ++i) {
        auto nextState(s);
        execute(nextState, moves[i]);
        if (nextState.state == 1) continue; // explode without enemy converted
        const int_fast16_t value = minVal(nextState);
        if (value > minimax) {
            minimax = value;
            if (!s.level) choiceIndex = moves[i];
        }
        if (s.hasBeta && value >= s.beta) return value;
        if (!s.hasAlpha || value > s.alpha) {s.hasAlpha = true; s.alpha = value;}
    }
    return minimax;
};

int_fast16_t Student::minVal(GameState& s) {
    if (s.level == gameTreeLevel || s.state >= 2) {return scoring(s);}
    int_fast16_t minimax = 32767;
    const auto moves = availableMoves(s);
    for (int_fast8_t i = 0, size = moves.size() ; i != size ; ++i) {
        auto nextState(s);
        execute(nextState, moves[i]);
        if (nextState.state == 1) continue; // explode without enemy converted
        const int_fast16_t value = maxVal(nextState);
        if (value < minimax) minimax = value;
        if (s.hasAlpha && value <= s.alpha) return value;
        if (!s.hasBeta || value < s.beta) {s.hasBeta = true; s.beta = value;}
    }
    return minimax;
};

inline const vector<int_fast8_t> Student::availableMoves(const GameState& s) const{
    vector<int_fast8_t> moves;
    // if (rounds+s.level <= 5) {
    //     for (int_fast8_t i = 0 ; i < 12 ; ++i)
    //         if ((s.cells[OpeningSeq[i]].color == White || s.cells[OpeningSeq[i]].color == s.currentPlayerColor)
    //                 && s.cells[OpeningSeq[i]].mass < critical[OpeningSeq[i]])
    //             moves.push_back(s.cells[OpeningSeq[i]].index);
    // } else {
    //     for (int_fast8_t i = 0 ; i < 30 ; ++i) {
    //         if ((s.cells[pmSeq[i]].color == White || s.cells[pmSeq[i]].color == s.currentPlayerColor)
    //                 && s.cells[pmSeq[i]].mass < critical[pmSeq[i]])
    //             moves.push_back(pmSeq[i]);
    //     }
    // }
    for (int_fast8_t i = 0; i < 30 ; ++i) {
        if (s.cells[i].color == White || s.cells[i].color == s.currentPlayerColor)
            moves.push_back(i);
    }
    return moves;
};

inline void Student::execute(GameState& s, const int_fast8_t index) {
    ++execCount;
    ++s.level; // update Game tree level for this GameState
    queue<int_fast8_t> addMassList;
    addMassList.push(index);
    bool exploded = false; //, convertEnemy = false;
    int_fast16_t converted = 0;

    while(!addMassList.empty()) {
        Cell &c = s.cells[addMassList.front()]; addMassList.pop();
        ++c.mass;
        if (c.color == s.nextPlayerColor) ++converted;
        if (c.color != s.currentPlayerColor) c.color = s.currentPlayerColor;
        if (c.mass == critical[c.index]) {
            c.color = Black;
            const auto &adjs = adjIndices[c.index];
            for (int_fast8_t i = 0, size = adjs.size() ; i != size ; ++i) {
                if (s.cells[adjs[i]].color != Black) addMassList.push(adjs[i]);
            }
            if (!exploded) exploded = true;
    }}
    
    if (exploded) {
        if (!converted) {
            if (rounds+s.level<15) {s.state = 1; return;} // bad move
        } else {
            if (s.currentPlayerColor == Blue) s.convertRed += converted;
            else s.convertBlue += converted;
            bool hasPlayerColor = false, hasEnemyColor = false;
            for (int_fast8_t i = 0 ; i < 30 ; ++i) {
                if (!hasPlayerColor && s.cells[i].color == playerColor) hasPlayerColor = true;
                if (!hasEnemyColor && s.cells[i].color == enemyColor) hasEnemyColor = true;
                if (hasPlayerColor && hasEnemyColor) break;
            }
            if (hasPlayerColor && !hasEnemyColor) {s.state = 2; return;} // player win
            if (hasEnemyColor && !hasPlayerColor) {s.state = 3; return;} // enemy win
    }}

    s.swapColor();
};

inline int_fast16_t Student::scoring(GameState& s) {
    if (s.state == 2) return 32767; // GameOver, player win
    else if (s.state == 3) return -32767; // GameOver, player lose

    s.swapColor();
    // short score = 0;
    // for (int_fast8_t i = 0 ; i < 30 ; ++i) {
    //     if (s.cells[i].color != playerColor) continue;
    //     const auto &adjs = adjIndices[i];
    //     int_fast8_t criticalEmeny = 0;
    //     for (int_fast8_t j = 0, size = adjs.size() ; j != size ; ++j) {
    //         if (s.cells[j].color == enemyColor && 
    //             s.cells[j].mass == critical[j]-1) ++criticalEmeny;
    //     }
    //     if (criticalEmeny) {
    //         score -= criticalEmeny * (5-critical[i]);;
    //     } else {
    //         bool found = false;
    //         if (i == 0 || i == 5 || i == 24 || i == 29) score += 3;
    //         else if (i == 1 || i ==2 || i ==3 || i ==4 || i ==6 || i ==11 || i ==12 ||
    //             i ==17 || i ==18 || i ==23 || i ==25 || i ==26 || i ==28 || i ==28)
    //             score += 2;
    //         if (s.cells[i].mass == critical[i]-1) score += 2;
    //     }
    //     score += s.cells[i].mass;
    // }

    int_fast16_t playerMass = 0, enemyMass = 0;
    for (int_fast8_t i = 0 ; i < 30 ; ++i) {
        if (s.cells[i].color == playerColor) playerMass += s.cells[i].mass;
        else if (s.cells[i].color == enemyColor) enemyMass += s.cells[i].mass;
    }
    int_fast16_t playerCorner = 0, playerEdge = 0, enemyCorner = 0, enemyEdge = 0;
    for (int_fast8_t i = 0 ; i < 4 ; ++i) {
        if (s.cells[corners[i]].color == playerColor) playerCorner += 3;
        else if (s.cells[corners[i]].color == enemyColor) enemyCorner += 3;
    }
    for (int_fast8_t i = 0 ; i < 14 ; ++i) {
        if (s.cells[edges[i]].color == playerColor) playerCorner += 1;
        else if (s.cells[edges[i]].color == enemyColor) enemyCorner += 1;
    }
    short score = (playerMass*5 + playerCorner + playerEdge)
        - (enemyMass*5 + enemyCorner + enemyEdge);
    // if (s.currentPlayerColor == Red) score += (s.convertBlue - s.convertRed)*2;
    // else score += (s.convertRed - s.convertBlue)*2;
    return score;
};