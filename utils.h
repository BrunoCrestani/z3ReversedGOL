#ifndef UTILS_H
#define UTILS_H

#include <vector>

void readMatrix(std::vector<std::vector<int>>& matrix);
int verifyGameState(std::vector<std::vector<int>>& currentState, std::vector<std::vector<int>>& nextState);

#endif // UTILS_H