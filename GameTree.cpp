#include <chrono>
#include <queue>
#include <vector>
#include "GameTree.hpp"

using namespace std;

void Student::makeMove(int Record[5][6], int Max[5][6], Color color[5][6], Color inputColor) {
    ++rounds;
    if (inited) update(Record, color, inputColor);
    else init(Record, Max, color, inputColor);
    if (rounds > 3) gameTreeLevel = 6;
    else if (rounds > 5) gameTreeLevel = 7;
    else if (rounds > 7) gameTreeLevel = 8;
    else if (rounds > 9) gameTreeLevel = 9;
    else if (rounds > 11) gameTreeLevel = 10;
    else if (rounds > 13) gameTreeLevel = 11;

    constexpr auto &&now = std::chrono::high_resolution_clock::now;
    auto start = now();

    auto root = make_shared<State>(cells, 0, playerColor, enemyColor);
    int max = maxVal(root, 0, 0, false, false);
    Cell &c = cells[root->choiceIndex];
    x = c.i; y = c.j;
    
    chrono::duration<double> elapsed = now() - start;
    cout << " -- Round " << rounds << ", GameTree Complete in: " << (int) (elapsed.count() * 1000) << " ms." << endl;

};
// Any Code You Want to Add
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

int Student::maxVal(shared_ptr<State> s, const int alpha, const int beta,
        const bool hasAlpha, const bool hasBeta) {
    if (s->treeLevel == gameTreeLevel) {return s->score;}
    if (hasAlpha) {s->hasAlpha = true; s->alpha = alpha;}
    if (hasBeta) {s->hasBeta = true; s->beta = beta;}

    s->minmax = -2147483647-1;
    vector<short> rawMoves = rawPossibleMoves(s); // make better move for player in front of vector
    for (short i = 0 ; i < rawMoves.size() ; ++i) {
        auto child = make_shared<State>(*s);
        nextMove(child, rawMoves[i]);
        if (child->isBadMove) continue;
        int value = minVal(child, s->alpha, s->beta, s->hasAlpha, s->hasBeta);
        if (value > s->minmax) {s->minmax = value; s->choiceIndex = rawMoves[i];}
        if (s->hasBeta && value >= s->beta) {return value;}
        if (!s->hasAlpha || value > s->alpha) {s->hasAlpha = true; s->alpha = value;}
    }

    return s->minmax;
};

int Student::minVal(shared_ptr<State> s, const int alpha, const int beta,
        const bool hasAlpha, const bool hasBeta) {
    if (s->treeLevel == gameTreeLevel) {return s->score;}
    if (hasAlpha) {s->hasAlpha = true; s->alpha = alpha;}
    if (hasBeta) {s->hasBeta = true; s->beta = beta;}

    s->minmax = 2147483647;
    vector<short> rawMoves = rawPossibleMoves(s);
    for (short i = 0 ; i < rawMoves.size() ; ++i) {
        auto child = make_shared<State>(*s);
        nextMove(child, rawMoves[i]);
        if (child->isBadMove) continue;
        int value = maxVal(child, s->alpha, s->beta, s->hasAlpha, s->hasBeta);
        if (value < s->minmax) {s->minmax = value; s->choiceIndex = rawMoves[i];}
        if (s->hasAlpha && value <= s->alpha) {return value;}
        if (!s->hasBeta || value < s->beta) {s->hasBeta = true; s->beta = value;}

    }
    return s->minmax;
};

vector<short> Student::rawPossibleMoves(const shared_ptr<State> s) {
    vector<short> moves;
    if (rounds+s->treeLevel <= 4) {
        for (short i = 0 ; i < 12 ; ++i) {
            Cell &c = s->cells[OpeningSeq[i]];
            if (c.color == White || c.color == s->currentPlayerColor)
                moves.push_back(c.index);
        }
    } else {
        for (short i = 0 ; i < 30 ; ++i) {
            Cell &c = s->cells[pmSeq[i]];
            if (c.color == White || c.color == s->currentPlayerColor)
                moves.push_back(c.index);
        }
    }
    return moves;
};

void Student::nextMove(shared_ptr<State> state, const short rawMoveIndex) {
    State &s = *state;
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

    scoring(state, exploded, convertEnemy);

    s.choiceIndex = rawMoveIndex;
    const Color _tmp = s.currentPlayerColor;
    s.currentPlayerColor = s.nextPlayerColor;
    s.nextPlayerColor = _tmp;
};

void Student::scoring(shared_ptr<State> s, const bool exploded, const bool convertEnemy) {
    int playerMass = 0, enemyMass = 0;
    for (int i = 0 ; i < 30 ; ++i) {
        if (s->cells[i].color == playerColor) playerMass += s->cells[i].mass;
        else if (s->cells[i].color == enemyColor) enemyMass += s->cells[i].mass;
    }
    int playerCorner = 0, playerEdge = 0, enemyCorner = 0, enemyEdge = 0;
    const short corners[4] {0, 5, 24, 29};
    for (const short& i : corners) {
        if (s->cells[i].color == playerColor) playerCorner += 3;
        else if (s->cells[i].color == enemyColor) enemyCorner += 3;
    }
    const short edges[14] {1, 2, 3, 4, 6, 11, 12, 17, 18, 23, 26, 26, 27, 28};
    for (const short& i : edges) {
        if (s->cells[i].color == playerColor) playerCorner += 1;
        else if (s->cells[i].color == enemyColor) enemyCorner += 1;
    }

    if (convertEnemy) {
        if (!enemyMass && s->currentPlayerColor == playerColor) {
            s->score = 2147483647;
            s->gameOver = true;
            return;
        } else if (!playerMass && s->currentPlayerColor == enemyColor) {
            s->score = -2147483647-1;
            s->gameOver = true;
            return;
        }
    }
    s->score = (playerMass*5 + playerCorner + playerEdge)
        - (enemyMass*5 + enemyCorner + enemyEdge);
};