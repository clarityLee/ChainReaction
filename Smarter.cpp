#include <vector>
#include <queue>
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
    int gameTreeLevel = 4;
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
    int maxVal(State&, const int alpha, const int beta,
        const bool hasAlpha, const bool hasBeta);
    int minVal(State&, const int alpha, const int beta,
        const bool hasAlpha, const bool hasBeta);
    vector<short> rawPossibleMoves(State&);
    void nextMove(State&, const short rawMoveIndex);
    void scoring(State&, const bool exploded, const bool convertEnemy);
};

void Student::makeMove(int Record[5][6], int Max[5][6], Color color[5][6], Color inputColor) {
    ++rounds;
    if (inited) update(Record, color, inputColor);
    else init(Record, Max, color, inputColor);
    if (rounds == 5) gameTreeLevel = 5;

    State root(cells, 0, playerColor, enemyColor);
    int max = maxVal(root, 0, 0, false, false);
    if (root.choiceIndex == -1) {
        if (gameTreeLevel == 1) {
            vector<short> moves = rawPossibleMoves(root);
            root.choiceIndex = moves[0];
        } else {
            --gameTreeLevel;
            makeMove(Record, Max, color, inputColor);
        }
    } else {
        Cell &c = cells[root.choiceIndex];
        x = c.i; y = c.j;
    }
};
int Student::getX() {
    return x;
};
int Student::getY() {
    return y;
};
void Student::init(int masses[5][6], int critical[5][6], Color color[5][6], Color inputColor) {
    
    cells.clear(); cells.reserve(30);

    for (int i = 0 ; i < 5 ; ++i) for (int j = 0 ; j < 6 ; ++j) {
        cells.emplace_back(cells.size(), i, j, masses[i][j], critical[i][j], color[i][j]);
    }
    adjIndices.clear(); adjIndices.reserve(30); adjIndices.resize(30);
    for (short index = 0 ; index < 30 ; ++index) {
        Cell &c = cells[index];
        std::vector<short> &adjs = adjIndices[index];
        if (c.i > 0) adjs.push_back(calcIndex(c.i-1, c.j));
        if (c.i < 4) adjs.push_back(calcIndex(c.i+1, c.j));
        if (c.j > 0) adjs.push_back(calcIndex(c.i, c.j-1));
        if (c.j < 5) adjs.push_back(calcIndex(c.i, c.j+1));
    }
    update(masses, color, inputColor);
    inited = true;
};

void Student::update(int masses[5][6], Color color[5][6], Color inputColor) {
    playerColor = inputColor;
    if (playerColor == Blue) enemyColor = Red;
    else enemyColor = Blue;
    for (int i = 0 ; i < 30 ; ++i) {
        Cell &c = cells[i];
        if (c.mass != masses[c.i][c.j]) c.mass = masses[c.i][c.j];
        if (c.color != color[c.i][c.j]) c.color = color[c.i][c.j];
    }
};

inline short Student::calcIndex(const short i , const short j) const {
    return i*6+j;
};

int Student::maxVal(State& s, const int alpha, const int beta,
        const bool hasAlpha, const bool hasBeta) {
    if (s.treeLevel == gameTreeLevel) {return s.score;}
    if (hasAlpha) {s.hasAlpha = true; s.alpha = alpha;}
    if (hasBeta) {s.hasBeta = true; s.beta = beta;}

    s.minmax = -2147483647-1;
    vector<short> rawMoves = rawPossibleMoves(s); // make better move for player in front of vector
    for (short i = 0 ; i < rawMoves.size() ; ++i) {
        State child(s);
        nextMove(child, rawMoves[i]);
        if (child.isBadMove) continue;
        int value = minVal(child, s.alpha, s.beta, s.hasAlpha, s.hasBeta);
        if (value > s.minmax) {s.minmax = value; s.choiceIndex = rawMoves[i];}
        if (s.hasBeta && value >= s.beta) {return value;}
        if (!s.hasAlpha || value > s.alpha) {s.hasAlpha = true; s.alpha = value;}
    }

    return s.minmax;
};

int Student::minVal(State& s, const int alpha, const int beta,
        const bool hasAlpha, const bool hasBeta) {
    if (s.treeLevel == gameTreeLevel) {return s.score;}
    if (hasAlpha) {s.hasAlpha = true; s.alpha = alpha;}
    if (hasBeta) {s.hasBeta = true; s.beta = beta;}

    s.minmax = 2147483647;
    vector<short> rawMoves = rawPossibleMoves(s);
    for (short i = 0 ; i < rawMoves.size() ; ++i) {
        State child(s);
        nextMove(child, rawMoves[i]);
        if (child.isBadMove) continue;
        int value = maxVal(child, s.alpha, s.beta, s.hasAlpha, s.hasBeta);
        if (value < s.minmax) {s.minmax = value; s.choiceIndex = rawMoves[i];}
        if (s.hasAlpha && value <= s.alpha) {return value;}
        if (!s.hasBeta || value < s.beta) {s.hasBeta = true; s.beta = value;}

    }
    return s.minmax;
};

vector<short> Student::rawPossibleMoves(State& s) {
    vector<short> moves;
    if (rounds+s.treeLevel <= 4) {
        for (short i = 0 ; i < 12 ; ++i) {
            Cell &c = s.cells[OpeningSeq[i]];
            if (c.color == White || c.color == s.currentPlayerColor)
                moves.push_back(c.index);
        }
    } else {
        for (short i = 0 ; i < 30 ; ++i) {
            Cell &c = s.cells[pmSeq[i]];
            if (c.color == White || c.color == s.currentPlayerColor)
                moves.push_back(c.index);
        }
    }
    return moves;
};

void Student::nextMove(State& s, const short rawMoveIndex) {
    ++s.treeLevel;
    queue<short> addList;
    addList.push(rawMoveIndex);
    bool exploded = false, convertEnemy = false;

    while(!addList.empty()) {
        Cell &c = s.cells[addList.front()]; addList.pop();
        if (c.color == Black) continue;
        ++c.mass;
        if (c.color == s.nextPlayerColor && !convertEnemy) convertEnemy = true;
        if (c.color != s.currentPlayerColor) c.color = s.currentPlayerColor;
        if (c.mass == c.critical) {
            c.color = Black;
            const std::vector<short> &adjs = adjIndices[c.index];
            for (short i = 0 ; i < adjs.size(); ++i) {
                addList.push(adjs[i]);
            }
            if (!exploded) exploded = true;
        }
    }
    
    if (exploded && !convertEnemy) {
        s.isBadMove = true;
        return;
    }

    scoring(s, exploded, convertEnemy);

    s.choiceIndex = rawMoveIndex;
    const Color _tmp = s.currentPlayerColor;
    s.currentPlayerColor = s.nextPlayerColor;
    s.nextPlayerColor = _tmp;
};

void Student::scoring(State& s, const bool exploded, const bool convertEnemy) {
    int playerMass = 0, enemyMass = 0;
    for (int i = 0 ; i < 30 ; ++i) {
        if (s.cells[i].color == playerColor) {
            if (s.cells[i].mass > 1) {
                playerMass += s.cells[i].mass * 0.9;
                // playerMass += s.cells[i].mass;
            } else {
                playerMass += s.cells[i].mass;
            }
        }
        else if (s.cells[i].color == enemyColor) enemyMass += s.cells[i].mass;
    }
    int playerCorner = 0, playerEdge = 0, enemyCorner = 0, enemyEdge = 0;
    const short corners[4] {0, 5, 24, 29};
    for (const short& i : corners) {
        if (s.cells[i].color == playerColor) playerCorner += 3;
        else if (s.cells[i].color == enemyColor) enemyCorner += 3;
    }
    const short edges[14] {1, 2, 3, 4, 6, 11, 12, 17, 18, 23, 26, 26, 27, 28};
    for (const short& i : edges) {
        if (s.cells[i].color == playerColor) playerEdge += 1;
        else if (s.cells[i].color == enemyColor) enemyEdge += 1;
    }

    if (convertEnemy) {
        if (!enemyMass && s.currentPlayerColor == playerColor) {
            s.score = 2147483647;
            s.gameOver = true;
            return;
        } else if (!playerMass && s.currentPlayerColor == enemyColor) {
            s.score = -2147483647-1;
            s.gameOver = true;
            return;
        }
    }
    s.score = (playerMass*5 + playerCorner + playerEdge)
        - (enemyMass*5 + enemyCorner + enemyEdge);
};