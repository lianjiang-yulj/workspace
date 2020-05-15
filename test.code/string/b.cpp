#include <map>
#include <iostream>
#include <string>

int main(){

  std::string b = "b";
  std::string a = b;
 // char* p = (char*)b.c_str();
 // p[0] = 'a';
 b[0] = 'a';

  std::cout << a << " " << b << std::endl;

  char buf[10];
  printf("%s\n", buf);

  return 0;
}
