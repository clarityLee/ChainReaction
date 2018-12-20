#include <vector>
#include <memory>
#include <iostream>
using namespace std;

// enum Color{
//     White, // Initital 0
//     Blue, // Player One
//     Red, // Player Two
//     Black //Explosion
// };
enum class CellPosition {
    middle,
    edge,
    corner
};
class Cell {
public:
    short index = 0, i = 0, j = 0, 
        critical = 0 , mass = 0; // criticalMass, currentMass
    Color color;
    CellPosition pos = CellPosition::middle;

    Cell(short _index, short _i, short _j, short _mass, short _critical,
        CellPosition _pos, Color _color):
        index(_index), i(_i), j(_j),
        mass(_mass), critical(_critical), pos(_pos), color(_color) {};
};
class State {
public:
    short choice = -1;
    std::vector<Cell> cells;
    std::vector<shared_ptr<State>> children;
    float score = 0;
    float minmax = 0;
    Color current, next;
    bool hasChild = true;

    State() {};
    State(std::vector<Cell> _cells, Color _current, Color _next):
        cells(_cells), current(_current), next(_next) {};
    std::vector<short> calcPossible() const;
};

std::vector<short> State::calcPossible() const {
    std::vector<short> moves;
    for (short i = 0 ; i < cells.size() ; ++i) {
        if (cells[i].mass >= cells[i].critical) continue;
        if (cells[i].color == Black || cells[i].color == next) continue;
        if (cells[i].color == White || cells[i].color == current) moves.push_back(i);
    }
        
    return moves;
};

class Student {
public:
    void makeMove(int Record[5][6], int Max[5][6], Color color[5][6], Color inputColor){
        if (inited) update(Record, color, inputColor);
        else init(Record, Max, color, inputColor);

        std::vector<std::vector<shared_ptr<State>>> levels(gameTreeLevel+1);

        auto root = make_shared<State>();
        for (int i = 0 ; i < 30 ; ++i) {
            root->cells.push_back(cells[i]);
        }
        root->current = player;
        root->next = enemy;
        
        levels[0].push_back(root);
        for (int i = 0 ; i < gameTreeLevel; ++i) {
            makeLeaves(levels, i);    
        }
        // std::cout << endl
        //     << "N0:" << levels[0].size()
        //     << ", N1:" << levels[1].size()
        //     << ", N2:" << levels[2].size()
        //     << ", N3:" << levels[3].size()
        //     << ", N4:" << levels[4].size() << endl;
            // << ", N5:" << levels[5].size()
            // << ", N6:" << levels[6].size()
            // << ", N7:" << levels[7].size()
            // << ", N8:" << levels[8].size() << endl;
        
        calcMinmax(root);
        

        for (int i = root->children.size() - 1 ; i >= 0 ; --i) {
            if (root->minmax == root->children[i]->minmax) {
                x = cells[root->children[i]->choice].i;
                y = cells[root->children[i]->choice].j;
            }
        }

    };
    // Any Code You Want to Add
    int getX(){
        return x;
    };
    int getY(){
        return y;
    };
private:
    int x = 0, y = 0;
    int gameTreeLevel = 4;
    bool inited = false;
    Color player;
    Color enemy;
    std::vector<Cell> cells;
    std::vector<std::vector<short>> adjIndices;
    std::vector<State> allStates;
    // shared_ptr<State> root = nullptr;

    void init(int masses[5][6], int critical[5][6], Color color[5][6], Color inputColor);
    void update(int masses[5][6], Color color[5][6], Color inputColor);
    inline short calcIndex(const short i , const short j) const;
    void makeLeaves(std::vector<std::vector<shared_ptr<State>>> &levels, int level);
    bool react(State &state, short index);
    float calcMinmax(shared_ptr<State> state);
    void sortAsc(vector<float> &);
    void sortDesc(vector<float> &);
};

void Student::init(int masses[5][6], int critical[5][6], Color color[5][6], Color inputColor) {
    
    cells.clear(); cells.reserve(30);

    for (int i = 0 ; i < 5 ; ++i) for (int j = 0 ; j < 6 ; ++j) {
        CellPosition pos = CellPosition::middle;
        if (i == 0 || i == 4) pos = static_cast<CellPosition>(static_cast<int>(pos)+1);
        if (j == 0 || j == 5) pos = static_cast<CellPosition>(static_cast<int>(pos)+1);
        cells.emplace_back(cells.size(), i, j, masses[i][j], critical[i][j], pos, color[i][j]);
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
    player = inputColor;
    allStates.clear();
    if (player == Blue) enemy = Red;
    else enemy = Blue;
    for (int i = 0 ; i < 30 ; ++i) {
        Cell &c = cells[i];
        if (c.mass != masses[c.i][c.j]) c.mass = masses[c.i][c.j];
        if (c.color != color[c.i][c.j]) c.color = color[c.i][c.j];
    }
};

inline short Student::calcIndex(const short i , const short j) const {
    return i*6+j;
};

void Student::makeLeaves(std::vector<std::vector<shared_ptr<State>>> &levels, const int level) {
    std::vector<shared_ptr<State>> &currentLevel = levels[level];
    std::vector<shared_ptr<State>> &nextLevel = levels[level+1];

    
    for(int i = currentLevel.size() - 1 ; i >= 0 ; --i) {
        State &s = *currentLevel[i];
        if (!s.hasChild) continue;
        std::vector<short> possibleMoves = s.calcPossible();
        vector<float> scores;

        for (short j = possibleMoves.size() - 1 ; j >= 0 ; --j) {
            auto ss = make_shared<State>();
            for (int k = 0 ; k < 30; ++ k) {
                ss->cells.push_back(s.cells[k]);
            }
            ss->current = s.current;
            ss->next = s.next;
            bool add = react(*ss, possibleMoves[j]);
            if (add) {
                nextLevel.push_back(ss);
                s.children.push_back(ss);
                scores.push_back(ss->score);
            }
        }
        // std::cout << ", " << s.children.size();

        short limitSize = 15;
        if (scores.size() > limitSize) {
            vector<shared_ptr<State>> keep1;
            vector<shared_ptr<State>> keep2;
            if (s.current == player) {
                sortDesc(scores);
                float threashold = scores[limitSize-1];
                for (int i = 0 ; i < s.children.size() ; ++i) {
                    if (s.children[i]->score >= threashold) {
                        keep1.push_back(s.children[i]);
                        keep1.push_back(s.children[i]);
                    }
                }
            } else {
                sortAsc(scores);
                float threashold = scores[limitSize-1];
                for (int i = 0 ; i < s.children.size() ; ++i) {
                    if (s.children[i]->score <= threashold) {
                        keep1.push_back(s.children[i]);
                        keep1.push_back(s.children[i]);
                    }
                }
            }
            keep1.swap(s.children);
            keep2.swap(nextLevel);
        }
        std::vector<Cell>().swap(s.cells);
    }

    std::vector<shared_ptr<State>>().swap(currentLevel);
};

bool Student::react(State &s, short index) {
    std::vector<Cell*> addMassList;
    addMassList.push_back(&(s.cells[index]));
    bool exploded = false;
    bool enemyConverted = false;

    while(!addMassList.empty()) {
        Cell &c = *addMassList.back();
        if (c.color != Black) {
            ++c.mass;
            if (!enemyConverted && c.color == s.next) enemyConverted = true;
            if (c.color != s.current) c.color = s.current;
            if (c.mass == c.critical) {
                c.mass = 0;
                c.color = Black;
                const std::vector<short> &adjs = adjIndices[c.index];
                for (short i = 0 ; i < adjs.size(); ++i) {
                    addMassList.push_back(&s.cells[adjs[i]]);
                }
                if (!exploded) exploded = true;
            }
        }
        addMassList.pop_back();
    }
    s.choice = index;
    
    float playerScore = 0, enemyScore = 0;
    for (short i = 0 ; i < 30 ; ++ i) {
        float score = 1;
        if (s.cells[i].mass >= 2) score += 1;
        if (s.cells[i].mass >= 3) score += 1;
        if (s.cells[i].pos == CellPosition::corner) score += 0.5;
        if (s.cells[i].pos == CellPosition::edge) score += 0.05;
        if (s.cells[i].color == player) playerScore += score;
        else if (s.cells[i].color == enemy) enemyScore += score;
    }
    s.score = playerScore - enemyScore*4;
    
    if (exploded && playerScore == 0) {
        s.score = -2147483648;
        s.hasChild = false;
    }

    const Color _tmp = s.current;
    s.current = s.next;
    s.next = _tmp;

    if (exploded && !enemyConverted) return false;

    Cell &ch = s.cells[index];
    if (ch.pos == CellPosition::middle) {
        bool hasEmptyCornerEdge = false;
        
        for (int i = 0 ; i < 30 ; ++i) {
            if (s.cells[i].color == White && !hasEmptyCornerEdge) {
                hasEmptyCornerEdge = true;
                break;
            }
        }
        if (hasEmptyCornerEdge) {
            bool hasAdjEnemy = false;
            if (ch.i > 0 && s.cells[calcIndex(ch.i-1, ch.j)].color == s.next) hasAdjEnemy = true;
            if (!hasAdjEnemy && ch.i < 4 && s.cells[calcIndex(ch.i+1, ch.j)].color == s.next) hasAdjEnemy = true;
            if (!hasAdjEnemy && ch.j > 0 && s.cells[calcIndex(ch.i, ch.j-1)].color == s.next) hasAdjEnemy = true;
            if (!hasAdjEnemy && ch.j < 5 && s.cells[calcIndex(ch.i, ch.j+1)].color == s.next) hasAdjEnemy = true;
            if (!hasAdjEnemy) return false;
        }
    }
    return true;
};

float Student::calcMinmax(shared_ptr<State> s) {

    if (!s->hasChild || s->children.empty()) {
        s->minmax = s->score;
        return s->minmax;
    }

    std::vector<float> minmaxes;
    for (int i = s->children.size() - 1 ; i >= 0 ; --i) {
        minmaxes.push_back(calcMinmax(s->children[i]));
    }

    if (s->current == player) {
        s->minmax = -2147483648;
        for (int i = minmaxes.size() - 1 ; i >= 0 ; --i) {
            if (minmaxes[i] > s->minmax) s->minmax = minmaxes[i];
            
        }
    } else {
        s->minmax = 2147483648;
        for (int i = minmaxes.size() - 1 ; i >= 0 ; --i) {
            if (minmaxes[i] < s->minmax) s->minmax = minmaxes[i];
        }
    }
    return s->minmax;
};

void Student::sortAsc(vector<float> &v) {
    float n; int j;
    for (int i = 1 ; i < v.size() ; ++i) {
        n = v[i];
        for (j = i - 1 ; j >= 0 && v[j] > n ; --j) 
            v[j+1] = v[j];
        v[j+1] = n;
    }
};
void Student::sortDesc(vector<float> &v) {
    float n; int j;
    for (int i = 1 ; i < v.size() ; ++i) {
        n = v[i];
        for (j = i - 1 ; j >= 0 && v[j] < n ; --j) 
            v[j+1] = v[j];
        v[j+1] = n;
    }
};