#include "utils.h"
#include <iostream>
#include <vector>

void readMatrix(std::vector<std::vector<int>>& matrix) {
    for (size_t row = 0; row < matrix.size(); row++) {
        for (size_t col = 0; col < matrix[0].size(); col++) {
            std::cin >> matrix[row][col];
        }
    }
}

int verifyGameState(std::vector<std::vector<int>>& currentState, std::vector<std::vector<int>>& nextState) {
    // Check if matrices have the same dimensions
    if (currentState.size() != nextState.size() || (currentState.size() > 0 && currentState[0].size() != nextState[0].size())) {
        return -1;
    }

    int rows = currentState.size();
    if (rows == 0) return 0;
    int cols = currentState[0].size();

    // Calculate the next state based on Game of Life rules
    std::vector<std::vector<int>> calculatedNextState(rows, std::vector<int>(cols, 0));

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int livingNeighbors = 0;
            
            // Check all 8 neighboring cells
            for (int deltaRow = -1; deltaRow <= 1; ++deltaRow) {
                for (int deltaCol = -1; deltaCol <= 1; ++deltaCol) {
                    // Skip the cell itself
                    if (deltaRow == 0 && deltaCol == 0) continue;

                    int neighborRow = row + deltaRow;
                    int neighborCol = col + deltaCol;

                    // Check if neighbor is within matrix bounds
                    if (neighborRow >= 0 && neighborRow < rows && 
                        neighborCol >= 0 && neighborCol < cols) {
                        livingNeighbors += currentState[neighborRow][neighborCol];
                    }
                }
            }

            // Apply Game of Life rules
            if (currentState[row][col] == 1) {
                // Live cell survives with 2 or 3 neighbors
                calculatedNextState[row][col] = (livingNeighbors == 2 || livingNeighbors == 3) ? 1 : 0;
            } else {
                // Dead cell becomes alive with exactly 3 neighbors
                calculatedNextState[row][col] = (livingNeighbors == 3) ? 1 : 0;
            }
        }
    }

    // Compare calculated next state with provided next state
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            if (calculatedNextState[row][col] != nextState[row][col]) {
                return 0;
            }
        }
    }

    return 1;
}