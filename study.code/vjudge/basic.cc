#include <iostream>
using namespace std;

int mountain[10000][10000];
int height[10000][10000];
int r, c;

int compute(int i, int j) {
  if (height[i][j] != 0) {
    //cout << i << " " << j << " " << height[i][j]   << endl;
    return height[i][j];
  }
  int max = 0;
  if (i - 1 >= 0 && mountain[i - 1][j] < mountain[i][j]) {
    int up = compute(i - 1, j) + 1;
    if (up > max) max = up;
  }
  if (i + 1 < r && mountain[i + 1][j] < mountain[i][j]) {
    int down = compute(i + 1, j) + 1;
    if (down > max) max = down;

  }
  if (j - 1 >= 0 && mountain[i][j - 1] < mountain[i][j]) {
    int left = compute(i, j - 1) + 1;
    if (left > max) max = left;


  }
  if (j + 1 < c && mountain[i][j + 1] < mountain[i][j]) {
    int right = compute(i, j + 1) + 1;
    if (right > max) max = right;
  }
  if (max == 0) max = 1;
  height[i][j] = max;
  //cout << i << " " << j << " " << max << endl;
  return height[i][j];
}

int main() {

  while (cin >> r >> c) {
    for (int i = 0; i < r; i++) {
      for (int j = 0; j < c; j++) {
        cin >> mountain[i][j];
        height[i][j] = 0;
      }
    }

    int max_len = 1;
    for (int i = 0; i < r; i++) {
      for (int j = 0; j < c; j++) {
        if (height[i][j] == 0) {
          compute(i, j);
        }
        if (height[i][j] > max_len) {
          max_len = height[i][j];
        }
      }
    }


    cout << max_len << endl;
  }
  return 0;
}