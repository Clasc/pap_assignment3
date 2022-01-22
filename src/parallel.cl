__kernel void hello_world(__global char *string) {
  string[0] = 'H';
  string[1] = 'e';
  string[2] = 'l';
  string[3] = 'l';
  string[4] = 'o';
  string[5] = ',';
  string[6] = ' ';
  string[7] = 'W';
  string[8] = 'o';
  string[9] = 'r';
  string[10] = 'l';
  string[11] = 'd';
  string[12] = '!';
  string[13] = '\0';
}

int index_at(const int n, const int p, int x, int y, int z) {
  return x + n * (y + p * z);
}

__kernel void setMatrix(const int maxRow, const int maxCol, const int maxDepth,
                        __global float *A) {
  const int row = get_global_id(0);
  const int col = get_global_id(1);
  printf("maxCol: %i\n", maxCol);
  printf("maxDepth: %i\n", maxDepth);
  index_at(maxCol, maxDepth, row, col, 0);
  A[index_at(maxCol, maxDepth, row, col, 0)] = (float)row / ((float)col + 1.00);
  A[index_at(maxCol, maxDepth, row, col, 1)] = 1.00;
  A[index_at(maxCol, maxDepth, row, col, 2)] = (float)col / ((float)row + 1.00);
}