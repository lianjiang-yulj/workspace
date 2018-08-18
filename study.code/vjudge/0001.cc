#include <iostream>
#include <queue>
#include <cstdint>
#include <vector>
#include <vector>
#include <iostream>



using namespace std;

int32_t main() {
  priority_queue<int32_t, vector<int32_t>, greater<int32_t> > q;
  int32_t num;
  while (cin >> num) {
    cout << num << endl;
    q.push(num);
    if (q.size() > 3) q.pop();
  }

  vector<int32_t> ans;
  while (!q.empty()) {
    ans.push_back(q.top());
    q.pop();
  }

  while (!ans.empty()) {
    cout << ans.back() << endl;
    ans.pop_back();
  }


  return 0;
}


