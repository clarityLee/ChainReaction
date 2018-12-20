#include <iostream>
#include <chrono>
#include <vector>
#include <queue>
#include <fstream>
#include "ColorEnum.hpp"
#include "Raction.hpp"
#include "GameTree.hpp"
using namespace std;

class coordinate {
public:
    coordinate() {};
    coordinate(int _i, int _j) : i(_i), j(_j) {};
    int i, j;
};
class PCell {
public:
    int i , j, index, mass, critical;
    Color color;
    PCell() {};
};

class Platform {
public:
    Platform();
    void run();
    bool react(Color color, int x, int y);
    void outputLog();
    void printCriticals();

    vector<PCell> cells;
    queue<coordinate> coordLog;
    int rounds = 1;
    Color winnerColor = White;
    int masses[5][6];
    int criticals[5][6];
    Color colors[5][6];
};

Platform::Platform() {
    for (int i = 0 ; i < 5 ; ++i) for (int j = 0 ; j < 6 ; ++j) {
        masses[i][j] = 0;
        criticals[i][j] = 4;
        if (i == 0 || i == 4) --criticals[i][j];
        if (j == 0 || j == 5) --criticals[i][j];
        colors[i][j] = White;
    }
    // printCriticals();
};

void Platform::run() {
    RAction st1;
    Student st2;

    while(true) {
        // cout << "--st1.makeMove, ";
        st1.makeMove(masses, criticals, colors, Blue);
        int st1_x = st1.getX(), st1_y = st1.getY();
        // cout << "st1_x:" << st1_x << ", st1_y:" << st1_y << ", ";
        coordLog.emplace(st1_x, st1_y);
        ++rounds;
        if (react(Blue, st1_x, st1_y)) return;

        // cout << "--st2.makeMove, ";
        st2.makeMove(masses, criticals, colors, Red);
        int st2_x = st2.getX(), st2_y = st2.getY();
        // cout << "st2_x:" << st2_x << ", st2_y:" << st2_y << ", ";
        coordLog.emplace(st2_x, st2_y);
        ++rounds;
        if (react(Red, st2_x, st2_y)) return;
    }
};

bool Platform::react(Color color, int x, int y) {

    if (colors[x][y] == Black) {
        if (color == Red) winnerColor = Blue;
        else winnerColor = Red;
        // cout << " react complete." << endl;
        return true;
    }

    queue<coordinate> addMass;
    addMass.emplace(x, y);

    while(!addMass.empty()) {
        coordinate co = addMass.front(); addMass.pop();
        // cout << ", addMass: " << addMass.size();
        if (co.i == -1 || co.j == -1 || co.i == 5 || co.j == 6) continue;
        if (colors[co.i][co.j] == Black) continue;

        ++masses[co.i][co.j];
        colors[co.i][co.j] = color;
        if (masses[co.i][co.j] >= criticals[co.i][co.j]) {
            colors[co.i][co.j] = Black;
            addMass.emplace(co.i-1, co.j);
            addMass.emplace(co.i+1, co.j);
            addMass.emplace(co.i, co.j-1);
            addMass.emplace(co.i, co.j+1);
        }
    }
    // cout << " react complete." << endl;
    if (rounds <=2) return false;
    int blues = 0, reds = 0;
    for (int i = 0 ; i < 5 ; ++i) for (int j = 0 ; j < 6 ; ++j) {
        if (colors[i][j] == Blue) ++blues;
        else if (colors[i][j] == Red) ++reds;
    }
    if (color == Red) {
        if (blues == 0) {
            winnerColor = Red;
            return true;
        } else return false;
    }
    if (color == Blue) {
        if (reds == 0) {
            winnerColor = Blue;
            return true;
        } else return false;
    }
    return false;
};

void Platform::outputLog() {
    ofstream o("./output.log");
    o << "Start" << endl;
    int count = 1;
    while(!coordLog.empty()) {
        coordinate coord = coordLog.front(); coordLog.pop();
        o << "Player " << ((count++ % 2 == 1) ? "One" : "Two") 
            << " : (" << coord.i+1 << "," << coord.j+1 << ")" << endl;
    }
    o << "Player "<< (winnerColor == Blue ? "One" : "Two") <<" Win !" << endl;
    cout << "Player "<< (winnerColor == Blue ? "One" : "Two") <<" Win !" << endl;
    
};

void Platform::printCriticals() {
    for (int i = 0 ; i < 5 ; ++i) {
        for (int j = 0 ; j < 6 ; ++j) {
            cout << criticals[i][j] << ' ';
        }
        cout << endl;
    }
};

int main() {
    Platform p;
    constexpr auto &&now = std::chrono::high_resolution_clock::now;
    auto start = now();
    p.run();
    p.outputLog();
    chrono::duration<double> elapsed = now() - start;
    cout << " -- Program Complete in: " << (int) (elapsed.count() * 1000) << " ms." << endl;
};