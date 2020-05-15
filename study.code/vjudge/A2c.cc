#include <iostream>
using namespace std;

int rele[50001];

int find_parent(int stu) {
  if (rele[stu] != 0) {
    rele[stu] = find_parent(rele[stu]);
    return rele[stu];
  }

  else return stu;
}

void join_rele(int sa, int sb) {
  int pa = find_parent(sa);
  int pb = find_parent(sb);
  if (pa < pb) rele[pa] = pb;
  else if (pb < pa) rele[pb] = pa;

  // cout << pa << " " << pb << " " << rele[sa] << " " << rele[sb] << endl;
}

int main() {
  int n, m, no = 0;
  while (cin >> n >> m) {
    if (n == 0 && m == 0) break;
    ++no;
    for (int i = 1; i <= n; i++) rele[i] = 0;
    int a, b;
    for (int i = 1; i <= m; i++) {
      cin >> a >> b;
      join_rele(a, b);
    }

    int case_num = 0;
    for (int i = 1; i <= n; i++) {
      if (rele[i] == 0) case_num ++;
    }
    cout << "Case " << no << ": " << case_num << endl;
  }
  return 0;
}