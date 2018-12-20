#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include "Raction.hpp"
using namespace std;


void RAction::makeMove(int masses[5][6], int criticals[5][6],
         Color colors[5][6], Color inputColor) {

    if (!inited) init(masses, criticals, colors, inputColor);
    else update(masses, criticals, colors, inputColor);

    vector<RACell*> possibleMoves;
    for (int i = 0 ; i < 30 ; ++i) {
        RACell &c = cells[i];
        if (c.color == White || c.color == playerColor) possibleMoves.push_back(&c);
    }

    uniform_int_distribution<int> uniDist(0, possibleMoves.size() - 1);
    // uniDist.param(uniform_int_distribution<int>::param_type(0, possibleMoves.size() - 1));

    RACell &c = *possibleMoves[uniDist(randomGenerator)];
    x = c.i;
    y = c.j;
};
void RAction::init(int masses[5][6], int criticals[5][6], Color colors[5][6], Color inputColor) {
    for (int i = 0 ; i < 5 ; ++i) for (int j = 0 ; j < 6 ; ++j)
        cells.emplace_back(cells.size(), i ,j , masses[i][j], criticals[i][j], colors[i][j]);
    playerColor = inputColor;
    inited = true;
};
void RAction::update(int masses[5][6], int criticals[5][6], Color colors[5][6], Color inputColor) {
    for (int i = 0 ; i < 30 ; ++i) {
        RACell &c = cells[i];
        if (c.mass != masses[c.i][c.j]) c.mass = masses[c.i][c.j];
        if (c.color != colors[c.i][c.j]) c.color = colors[c.i][c.j];
    }
};
int RAction::getX() {
    return x;
};
int RAction::getY() {
    return y;
};