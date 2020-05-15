#include <string>

#include <iostream>
#include <cstdio>
#include <stdint.h>

using namespace std;

typedef union {
  int32_t big;
  char small[4];
} CPU;


int main()

{
  string str1 = "copy on write test!";
  string str2 = str1;
  cout << "begin write:" << endl;
  cout << "c_str() adress:" << endl;
  printf ("str1's address: %p\n", str1.c_str());
  printf ("str2's address: %p\n", str2.c_str());
  cout << "str[0] adress:" << endl;
  printf ("str1's address: %p\n", &str1[0]);
  printf ("str2's address: %p\n", &str2[0]);
  str1[1]='w';
  cout << "after write:" << endl;
  cout << "c_str() adress:" << endl;
  printf ("str1's address: %p\n", str1.c_str());
  printf ("str2's address: %p\n", str2.c_str());
  cout << "str[0] adress:" << endl;
  printf ("str1's address: %p\n", &str1[0]);
  printf ("str2's address: %p\n", &str2[0]);

  CPU u;
  u.big = 0x01020304;

  if (u.small[0] == 0x04)
    printf("little endian: %d\n", u.small[0]);
  cout << oct << u.small[0] << endl;

  int num = 0x0102;
  char* p = (char*)&num;
  printf("%d\n", p[0]);
  printf("%d\n", p[1]);
  return 0;

}
