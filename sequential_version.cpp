#include <fstream>
#include <stdio.h>
#include <math.h>
// const int m = 8192, n = 8192, p = 3;
const int m = 100, n = 100, p = 3;
float A[m][n][p];

int main() {
	//loop to fill in every row
	for (int i = 0; i < m; i++) {
		//loop to fill in every column
		for (int j = 0; j < n; j++) {
			//loop to fill in each 2D matrix
			for (int k = 0; k < p; k++) {
				//for matrix k=0
				if (k == 0) {
					A[i][j][k] = (float)i / ((float)j + 1.00);
				}
				//for matrix k=1
				else if (k == 1) {
					A[i][j][k] = 1.00;
				}
				//for matrix k=2
				else {
					A[i][j][k] = (float)j / ((float)i + 1.00);
				}
			}
		}
	}

	//Iteration count
	for (int t = 0; t < 24; t++) {
		//each row - beware first row and last row not to be updated therefore from 1...8190
		for (int i = 1; i < m - 1; i++) {
			//each column
			for (int j = 0; j < n; j++) {
				//only matrix k=1 is updated
				A[i][j][1] = A[i][j][1] + (1 / (sqrt(A[i + 1][j][0] + A[i - 1][j][2])));
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
			ofs << std::endl;
		}
	}

	ofs.close();
	return 0;
}