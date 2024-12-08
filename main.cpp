#include <iostream>
#include <vector>
#include <stdexcept>
#include <z3++.h>
#include "utils.h"

using namespace std;
using namespace z3;

// Calculate neighbor sum for Z3 expressions
expr calculateNeighborSum(context& context, const vector<vector<expr>>& grid, int rows, int cols, int row, int col) {
    expr neighborSum = context.int_val(0);
    for (int deltaRow = -1; deltaRow <= 1; deltaRow++) {
        for (int deltaCol = -1; deltaCol <= 1; deltaCol++) {
            if (deltaRow == 0 && deltaCol == 0) continue;

            int neighborRow = row + deltaRow;
            int neighborCol = col + deltaCol;

            if (neighborRow >= 0 && neighborRow < rows && 
                neighborCol >= 0 && neighborCol < cols) {
                neighborSum = neighborSum + 
                    ite(grid[neighborRow][neighborCol], 
                        context.int_val(1), 
                        context.int_val(0));
            }
        }
    }
    return neighborSum;
}

// Calculate neighbor sum in the original matrix
int calculateNeighborSumOriginal(const vector<vector<int>>& matrix, int rows, int cols, int row, int col) {
    int neighborSum = 0;
    for (int deltaRow = -2; deltaRow <= 2; deltaRow++) {
        for (int deltaCol = -2; deltaCol <= 2; deltaCol++) {
            if (deltaRow == 0 && deltaCol == 0) continue;

            int neighborRow = row + deltaRow;
            int neighborCol = col + deltaCol;

            if (neighborRow >= 0 && neighborRow < rows &&
                neighborCol >= 0 && neighborCol < cols) {
                neighborSum += matrix[neighborRow][neighborCol];
            }
        }
    }
    return neighborSum;
}

// Main problem-solving function
bool solveGameOfLife(int rows, int cols, vector<vector<int>>& currentState) {
    // Input validation
    if (rows <= 0 || cols <= 0) {
        throw invalid_argument("Matrix dimensions must be positive");
    }

    // Solver configuration
    config solverConfig;
    context context(solverConfig);
    optimize solver(context);

    // Create SAT variables
    vector<vector<expr>> cellGrid(rows, vector<expr>(cols, context.bool_val(false)));
    for (int i = 0; i < rows; i++) 
        for (int j = 0; j < cols; j++) 
            cellGrid[i][j] = context.bool_const(
                ("cell_" + to_string(i) + "_" + to_string(j)).c_str()
            );

    // Apply constraints for each cell
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            expr neighborSum = calculateNeighborSum(context, cellGrid, rows, cols, i, j);
            int neighborSumOriginal = calculateNeighborSumOriginal(currentState, rows, cols, i, j);

            if (currentState[i][j] == 1) {
                solver.add((cellGrid[i][j] && (neighborSum == 2 || neighborSum == 3)) ||
                           (!cellGrid[i][j] && neighborSum == 3));
            } else {
                if (neighborSumOriginal == 0) {
                    solver.add(!cellGrid[i][j]);
                } else {
                    solver.add((!cellGrid[i][j] && neighborSum != 3) ||
                               (cellGrid[i][j] && (neighborSum < 2 || neighborSum > 3)));
                }
            }
        }
    }

    // Minimize live cells
    expr totalLiveCells = context.int_val(0);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            totalLiveCells = totalLiveCells + 
                ite(cellGrid[i][j], context.int_val(1), context.int_val(0));
        }
    }
    solver.minimize(totalLiveCells);

    if (solver.check() == sat) {
        model solverModel = solver.get_model();
        
        vector<vector<int>> previousState(rows, vector<int>(cols, 0));

        cout << rows << " " << cols << "\n";        
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                expr evaluatedCell = solverModel.eval(cellGrid[i][j], true);
                previousState[i][j] = evaluatedCell.is_true() ? 1 : 0;
                cout << previousState[i][j];
                if (j < cols - 1) cout << " ";
            }
            cout << "\n";
        }

#ifdef VERIFY
        // Verify solution
        int verificationResult = verifyGameState(previousState, currentState);
        
        cout << "\nVerification: ";
        switch (verificationResult) {
            case 1:
                cout << "Valid solution! Previous state leads to current state." << endl;
                return true;
            case 0:
                cout << "Invalid solution. Previous state does not generate current state." << endl;
                return false;
            case -1:
                cout << "Verification error (incompatible dimensions)." << endl;
                return false;
        }
#endif
    } else {
        // Print the size of the matrix
        cout << rows << " " << cols << "\n";

        // Print a fully zeroed matrix
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                cout << "0";
                if (j < cols - 1) cout << " ";
            }
            cout << "\n";
        }
        return false;
    }
}

int main() {
    try {
        int rows, cols;
        cin >> rows >> cols;

        vector<vector<int>> matrix(rows, vector<int>(cols));
        readMatrix(matrix);

        solveGameOfLife(rows, cols, matrix);
    } 
    catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
