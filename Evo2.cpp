#include <vector>
#include <queue>
#include "ColorEnum.hpp"
using namespace std;
enum class Colour {WHI, BLU, RED, BLK};
class Cell {
public:
    short index = 0, mass = 0; // criticalMass, currentMass
    Colour color;
};
class GameState {
public:
    bool hasAlpha = false, hasBeta = false;
    short level = 0, state = 0, alpha = 0, beta = 0;
    Colour currentPlayerColor, nextPlayerColor;
    short blueConverted = 0, redConverted = 0;
    vector<Cell> cells;
    inline void swapPlayer();
    GameState(int mass[5][6], Color colors[5][6], Colour _c, Colour _n);
};
GameState::GameState(int mass[5][6], Color colors[5][6], Colour _c, Colour _n):
        cells(30), currentPlayerColor(_c), nextPlayerColor(_n) {
    for (short i = 0, index = 0; i < 5; ++i) for (short j = 0; j < 6; ++j, ++index) {
        cells[index].index = index;
        cells[index].mass = mass[i][j];
        switch(colors[i][j]) {
            case White: cells[index].color = Colour::WHI; break;
            case Red:   cells[index].color = Colour::RED; break;
            case Blue:  cells[index].color = Colour::BLU; break;
            case Black: cells[index].color = Colour::BLK; break;
        }
    }
};
inline void GameState::swapPlayer() {
    const Colour _tmp = currentPlayerColor; 
    currentPlayerColor = nextPlayerColor;
    nextPlayerColor = _tmp;
}
class Student {
public:
    void makeMove(int Record[5][6], int Max[5][6], Color color[5][6], Color inputColor);
    int getX();
    int getY();
private:
    short gameTreeLevel = 4, decisionIndex = -1, rounds = 0;
    int x = 0, y = 0;
    bool inited = false;
    Colour playerColor = Colour::WHI, enemyColor = Colour::WHI;
    vector<Cell> Cells;
    const short critical[30] {2, 3, 3, 3, 3, 2, 3, 4, 4, 4, 4,
        3, 3, 4, 4, 4, 4, 3, 3, 4, 4, 4, 4, 3, 2, 3, 3, 3, 3, 2};
    const short pmSeq[30] {0, 5, 24, 29, 2, 27, 12, 17, 7, 22, 10, 19, 25, 4, 6,
        23, 26, 3, 18, 11, 1, 28, 8, 21, 13, 16, 9, 20, 14, 15}; // possibleMoveSeq
    const int OpeningSeq[12] {0, 5, 24, 29, 1, 4, 6, 11, 18, 13, 25, 28};
    const vector<vector<short>> adjIndices {{6, 1}, {7, 0, 2}, {8, 1, 3}, {9, 2, 4},
        {10, 3, 5}, {11, 4}, {0, 12, 7}, {1, 13, 6, 8}, {2, 14, 7, 9}, {3, 15, 8, 10},
        {4, 16, 9, 11}, {5, 17, 10}, {6, 18, 13}, {7, 19, 12, 14}, {8, 20, 13, 15},
        {9, 21, 14, 16}, {10, 22, 15, 17}, {11, 23, 16}, {12, 24, 19}, {13, 25, 18, 20},
        {14, 26, 19, 21}, {15, 27, 20, 22}, {16, 28, 21, 23}, {17, 29, 22}, {18, 25},
        {19, 24, 26}, {20, 25, 27}, {21, 26, 28}, {22, 27, 29}, {23, 28}};
    short maxVal(GameState&);
    short minVal(GameState&);
    inline void execute(GameState&, const short moveIndex);
    inline short scoring(GameState&);
    inline const vector<short> availableMoves(const GameState&) const;
};
void Student::makeMove(int masses[5][6], int Max[5][6], Color colors[5][6], Color inputColor) {
    if (playerColor == Colour::WHI) {
        if (inputColor == Blue) {playerColor = Colour::BLU; enemyColor = Colour::RED;}
        else {playerColor = Colour::RED; enemyColor = Colour::BLU;}
    }
    if (++rounds == 5) gameTreeLevel = 5;

    GameState root(masses, colors, playerColor, enemyColor);
    decisionIndex = -1;
    maxVal(root);
    
    if (decisionIndex != -1) {
        x = decisionIndex / 6; y = decisionIndex % 6;
    } else {
        if (gameTreeLevel > 1) {
            --gameTreeLevel;
            makeMove(masses, Max, colors, inputColor);
        } else {
            auto moves = availableMoves(root);
            if (moves.empty()) {x = 0; y = 0;}
            else  {
                decisionIndex = moves[0];
                x = decisionIndex / 6; y = decisionIndex % 6;
            }
        }
    }
};
int Student::getX() {return x;};
int Student::getY() {return y;};

short Student::maxVal(GameState& s) {

    if (s.level == gameTreeLevel || s.state >= 2) {return scoring(s);}

    int maxValue = -32767;
    const auto moves = availableMoves(s);

    for (short i = 0, size = moves.size() ; i != size ; ++i) {
        auto nextState(s); execute(nextState, moves[i]);
        if (nextState.state == 1) continue;

        const short value = minVal(nextState);
        if (value > maxValue) {maxValue = value; if (!s.level) decisionIndex = moves[i];}
        if (s.hasBeta && value >= s.beta) {return value;}
        if (!s.hasAlpha || value > s.alpha) {s.hasAlpha = true; s.alpha = value;}
    }
    return maxValue;
};

short Student::minVal(GameState& s) {

    if (s.level == gameTreeLevel || s.state >= 2) {return scoring(s);}
    int minValue = 32767;

    const auto moves = availableMoves(s);
    for (short i = 0, size = moves.size() ; i != size ; ++i) {
        auto nextState(s); execute(nextState, moves[i]);
        if (nextState.state == 1) continue;

        const short value = maxVal(nextState);
        if (value < minValue) minValue = value;
        if (s.hasAlpha && value <= s.alpha) return value;
        if (!s.hasBeta || value < s.beta) {s.hasBeta = true; s.beta = value;}
    }
    return minValue;
};

inline const vector<short> Student::availableMoves(const GameState& s) const {
    vector<short> moves;
    if (rounds+s.level <= 4) {
        for (short i = 0 ; i < 12 ; ++i) {
            const Cell &c = s.cells[OpeningSeq[i]];
            if (c.color == Colour::WHI || c.color == s.currentPlayerColor)
                moves.push_back(c.index);
        }
    } else {
        for (short i = 0 ; i < 30 ; ++i) {
            const Cell &c = s.cells[pmSeq[i]];
            if (c.color == Colour::WHI || c.color == s.currentPlayerColor)
                moves.push_back(c.index);
        }
    }
    return moves;
};
void Student::execute(GameState& s, const short moveIndex) {
    ++s.level;
    queue<short> addMassList;
    addMassList.push(moveIndex);
    bool exploded = false;
    short converted = 0;

    while(!addMassList.empty()) {
        Cell &c = s.cells[addMassList.front()]; addMassList.pop();
        ++c.mass;
        if (c.color == s.nextPlayerColor) ++converted;
        if (c.color != s.currentPlayerColor) c.color = s.currentPlayerColor;
        if (c.mass == critical[c.index]) {
            c.color = Colour::BLK;
            const auto &adjs = adjIndices[c.index];
            for (short i = 0, size = adjs.size() ; i != size; ++i) {
                if (s.cells[adjs[i]].color != Colour::BLK) addMassList.push(adjs[i]);
            }
            if (!exploded) exploded = true;
    }}
    
    if (exploded) {
        if (!converted) {
            s.state = 1; return;
        } else {
            if (s.currentPlayerColor == Colour::BLU) s.redConverted += converted;
            else s.blueConverted += converted;
        }
        bool hasPlayer = false, hasEnemy = false;
        for (short i = 0 ; i < 30 ; ++i) {
            if (!hasPlayer && s.cells[i].color == playerColor) hasPlayer = true;
            if (!hasEnemy && s.cells[i].color == enemyColor) hasEnemy = true;
            if (hasPlayer && hasEnemy) break;
        }
        if (hasPlayer && !hasEnemy) {s.state = 2; return;} // player win
        if (hasEnemy && !hasPlayer) {s.state = 3; return;} // enemy win
        return;
    }

    s.swapPlayer();
};
inline short Student::scoring(GameState& s) {
    if (s.state == 2) return 32767; // GameOver. Score for player win.
    if (s.state == 3) return -32767; //GameOver. Score for enemy win.

    short playerScore = 0, enemyScore = 0;
    
    for (short i = 0 ; i < 30 ; ++i) {
        if (s.cells[i].color == playerColor) {
            playerScore += s.cells[i].mass * 5;
            if (critical[i] == 2) playerScore += 3;
            else if (critical[i] == 3) playerScore += 1;
        }
        else if (s.cells[i].color == enemyColor) {
            enemyScore += s.cells[i].mass * 5;
            if (critical[i] == 2) enemyScore += 3;
            else if (critical[i] == 3) enemyScore += 1;
        }
    }

    return playerScore - enemyScore;
};