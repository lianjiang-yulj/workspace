#include <map>
#include <iostream>
#include <tr1/unordered_map>
#include "basic/include/basic_time.h"

using namespace std;

int main() {
  map<string, string> m1;
  tr1::unordered_map<string, string> m2;

  std::string key = "key";
  std::string value = "value";

  for (int i = 0; i < 10; ++i) {
    m1.insert(std::make_pair(key, value));
    m2.insert(std::make_pair(key, value));

    key.append("k");
    value.append("v");
  }



  for (int i = 0; i < 10; i++) {
  key = "key";
  value = "value";
    int64_t begin = basic_time::TimeNow();
    for (int i = 0; i < 10000000; ++i) {
      m1.find(key);
    }
    int64_t end = basic_time::TimeNow();

    int64_t t1 = end - begin;
    begin = basic_time::TimeNow();
    for (int i = 0; i < 10000000; ++i) {
      m2.find(key);
    }
    end = basic_time::TimeNow();
    int64_t t2 = end - begin;
    cout << t2  << " " << t1 << endl;
    cout << (t2 <= t1) << endl;
    key.append("k");
    value.append("v");
  }

  return 0;
}
