#include <fstream>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <chrono>
using namespace std;

// sizes cannot be passed because of segmentation faults -> have to be static to be that big!
// stores computed matrix in file if filename is passed
double run_sequential(const char* save_to_file = nullptr) {
	// const int m = 8192, n = 8192, p = 3;
	// let the size of the matrix be divisible by 32
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			A[i][j][0] = (float)i / ((float)j + 1.00);
			A[i][j][1] = 1.00;
			A[i][j][2] = (float)j / ((float)i + 1.00);
		}
	}

	auto diff = measure([&] {
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
		});


	// save image
	if (save_to_file) {
		std::ofstream ofs(save_to_file, std::ofstream::out);
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
	}

	return diff;
}