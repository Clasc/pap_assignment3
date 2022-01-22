#include <fstream>
#include <stdio.h>
#include <math.h>

// pass sizes for dimensions
int run_sequential(const int m, const int n, const int p) {

	// const int m = 8192, n = 8192, p = 3;
	// let the size of the matrix be divisible by 32
	float A[m][n][p];
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			A[i][j][0] = (float)i / ((float)j + 1.00);
			A[i][j][1] = 1.00;
			A[i][j][2] = (float)j / ((float)i + 1.00);
		}
	}

	//Iteration count
	for (int t = 0; t < 24; t++) {
		//each row - beware first row and last row not to be updated therefore from 1...m-1
		for (int i = 1; i < m - 1; i++) {
			//each column
			for (int j = 0; j < n; j++) {
				//only matrix k=1 is updated
				A[i][j][1] += 1 / sqrt(A[i + 1][j][0] + A[i - 1][j][2]);
			}
		}
	}

	// save image
	std::ofstream ofs("sequential_matrix.txt", std::ofstream::out);
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < p; k++) {
				ofs << " " << A[i][j][k];
			}
			ofs << "    ";
		}
		ofs << std::endl;
	}

	ofs.close();
	return 0;
}