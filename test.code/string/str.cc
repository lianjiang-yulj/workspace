#include <string>

std::string str;

std::string f() {
  str = "a";
  printf("%p\n", str.data());
  return str;
}

int main() {
  printf("%p\n", f().data());
  return 0;
}
