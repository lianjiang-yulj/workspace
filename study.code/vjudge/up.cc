#include <iostream>
#include <string.h>
using namespace std;

int card[2][20];
int score[20][20][20][20];
int f[2][20][20];
int sum[20][20][20][20];
int sum2[2][20][20];

int compute(int a1, int a2, int b1, int b2) {
  if (a1 > a2) {
    if (b1 == b2) return f[1][b1][b2];
    else if (b1 < b2) {
      if (f[1][b1][b2] != 0) {
        return f[1][b1][b2];
      }
      return f[1][b1][b2] = max(sum2[1][b1][b2] - compute(a1, a2, b1 + 1, b2), sum2[1][b1][b2] - compute(a1, a2, b1, b2 - 1));
    } else {
      return 0;
    }
  } else if (b1 > b2) {
    if (a1 == a2) return f[0][a1][a2];
    else if (a1 < a2) {
      if (f[0][a1][a2] != 0) {
        return f[0][a1][a2];
      }
      return f[0][a1][a2] = max(sum2[0][a1][a2] - compute(a1 + 1, a2, b1, b2), sum2[0][a1][a2] - compute(a1, a2 - 1, b1, b2));
    } else {
      return 0;
    }
  } else {
    if (score[a1][a2][b1][b2] != 0) return score[a1][a2][b1][b2];
    /*
    return score[a1][a2][b1][b2] = max(max(card[0][a1] + compute(a1 + 1, a2, b1, b2), card[0][a2] + compute(a1, a2 - 1, b1, b2)),
                                       max(card[1][b1] + compute(a1, a2, b1 + 1, b2), card[1][b2] + compute(a1 , a2, b1, b2 - 1)));
                                       */

    int s1 = sum[a1][a2][b1][b2] - compute(a1 + 1, a2, b1, b2);
    int s2 = sum[a1][a2][b1][b2] - compute(a1, a2 - 1, b1, b2);
    int s3 = sum[a1][a2][b1][b2] - compute(a1, a2, b1 + 1, b2);
    int s4 = sum[a1][a2][b1][b2] - compute(a1 , a2, b1, b2 - 1);

    score[a1][a2][b1][b2] = max(max(s1, s2), max(s3, s4));
    return score[a1][a2][b1][b2];
  }
}

int main() {

  int T; cin >> T;
  for (int t = 0; t < T; t++) {
    int n; cin >> n;
    memset(f, 0, sizeof(f));
    memset(score, 0, sizeof(score));
    memset(sum, 0, sizeof(sum));
    memset(sum2, 0, sizeof(sum2));
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < n; j++) {
        cin >> card[i][j];
        f[i][j][j] = card[i][j];
        sum2[i][j][j] = card[i][j];
      }
    }



    for (int b1 = 0; b1 < n; b1++) {
      for (int b2 = b1 + 1; b2 < n; b2++) {
        sum2[1][b1][b2] = sum2[1][b1][b2 - 1] + card[1][b2];
        sum2[0][b1][b2] = sum2[0][b1][b2 - 1] + card[0][b2];
      }
    }
    for (int a1 = 0; a1 < n; a1++) {
      for (int a2 = a1; a2 < n; a2++) {
        for (int b1 = 0; b1 < n; b1++) {
          for (int b2 = b1; b2 < n; b2++) {
            sum[a1][a2][b1][b2] = sum2[0][a1][a2] + sum2[1][b1][b2];
          }
        }
      }
    }
    /*
        for (int a1 = 0; a1 < n; a1++) {
          for (int a2 = a1; a2 < n; a2++) {
            for (int b1 = 0; b1 < n; b1++) {
              for (int b2 = b1; b2 < n; b2++) {
                score[a1][a2][b1][b2] = compute(a1, a2, b1, b2);
              }
            }
          }
        }
        */

    //cout << score[0][n - 1][0][n - 1] << endl;
    cout << compute(0, n - 1, 0, n - 1) << endl;
  }
  return 0;
}