#include <iostream>
#include <map>
#include <string>
#include <functional>

using namespace std;

auto lambda = [](auto x, auto y) {return x + y;};

int main() {
  std::cout << lambda(1,2) << std::endl;
  map<int, std::string> m;
  m[1] = "1";
  m[2] = "hello";

  cout << m[2] << endl;

  [out = std::ref(std::cout << "Result from C code: " << lambda(1, 2))](){
    out.get() << ".\n";
  }();

  return 0;
}

class a {
 public:
  a() = default;
  ~a() = default;
};
