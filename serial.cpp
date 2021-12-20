
#include <math.h>
#include <stdio.h>
#include <iostream>

/**
 * @brief An iterative process with t = 24 iterations shall evaluate an equation, only for k=1, 0 < i < m-1, 0 < j < n-1
 */

float solve(float next, float last) {
    return 1.f / sqrt(next + last);
}

int main(int argc, char const* argv[]) {
    std::cout << "initialized with:" << std::endl;
    int m = 10, n = 10, p = 3;
    std::cout << "m:" << m << std::endl << "n:" << n << std::endl << "p:" << p << std::endl;

    float mat[m][n][p];

    //prefill the matrix
    std::cout << "fill array..." << std::endl;
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            mat[i][j][0] = i / (j + 1.f);
            mat[i][j][1] = 1.f;
            mat[i][j][2] = j / (i + 1.f);
        }
    }

    // solve equations for 1 iteration
    std::cout << "solve equation..." << std::endl;
    for (size_t i = 0; i < m - 1; i++) {
        for (size_t j = 0; j < n - 1; j++) {
            // calc val
            mat[i][j][1] += solve(mat[i + 1][j][0], mat[i - 1][j][2]);
        }
    }

    // print matrix
    for (size_t i = 0; i < m; i++) {
        std::cout << "[" << std::endl;
        for (size_t j = 0; j < n; j++) {
            std::cout << "  [" << std::endl << "    ";
            for (size_t k = 0; k < p; k++) {
                std::cout << mat[i][j][k] << ",";
            }
            std::cout << std::endl << "  ]" << std::endl;
        }
        std::cout << "]" << std::endl;
    }

    return 0;
}
