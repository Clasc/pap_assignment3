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

int index_at(int x, int y, int z) {
  int m = get_global_size(0);
  int n = get_global_size(1);
  return x + m * (y + n * z);
}

__kernel void setMatrix(__global float *A) {
  const int i = get_global_id(0);
  const int j = get_global_id(1);
  A[index_at(i, j, 0)] = (float)i / ((float)j + 1.00);
  A[index_at(i, j, 1)] = 1.00;
  A[index_at(i, j, 2)] = (float)j / ((float)i + 1.00);
}

__kernel void calculatMatrix(__global float *A_in) {
  const int i = get_global_id(0);
  const int j = get_global_id(1);
  if (i < 1 || i == get_global_size(0) - 1) {
    return;
  }
  A_in[index_at(i, j, 1)] +=
      1 / sqrt(A_in[index_at(i + 1, j, 0)] + A_in[index_at(i - 1, j, 2)]);
}