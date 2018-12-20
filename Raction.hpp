#pragma once
#ifndef _CHAINREACTION_RANDOM_
#define _CHAINREACTION_RANDOM_
#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include "ColorEnum.hpp"
using namespace std;

class RACell {
public:
    int i, j, index, mass, critical;
    Color color;
    RACell() {};
    RACell(int _index, int _i, int _j, int _mass, int _critical, Color _color):
        index(_index), i(_i), j(_j), mass(_mass), critical(_critical), color(_color) {};
};

class RAction {
public:
    void makeMove(int masses[5][6], int criticals[5][6], Color colors[5][6], Color inputColor);
    int getX();
    int getY();
    void init(int masses[5][6], int criticals[5][6], Color colors[5][6], Color inputColor);
    void update(int masses[5][6], int criticals[5][6], Color colors[5][6], Color inputColor);
    RAction() : randomGenerator(chrono::high_resolution_clock::now().time_since_epoch().count()) {};
private:
    bool inited = false;
    int x = 0, y = 0;
    Color playerColor;
    vector<RACell> cells;
    mt19937 randomGenerator;
};


#endif /*_CHAINREACTION_RANDOM_*/