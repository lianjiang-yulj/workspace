#include <iostream>
#include <map>
#include <string>

using namespace std;

auto lambda = [](auto x, auto y) {return x + y;};

int main() {
  std::cout << lambda(1,2) << std::endl;
  map<int, std::string> m;
  m[1] = "1";
  m[2] = "hello";

  cout << m[2] << endl;
  return 0;
}
