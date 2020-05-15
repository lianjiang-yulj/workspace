#include "Buffer.h"
#include <iostream>
using namespace std;

int main(){
  string s1 = "hello world";
  string s2 = "good morning";
  Buffer buf;
  assert(buf.writableSize() == 65535);
  buf.appendString(s1);
  buf.appendString(s2);

  string s = buf.readString();
  assert(s == s1);
  assert(buf.writableSize() == 65535 - 4 - 4 - s1.length() - s2.length());
  buf.shrink();
  assert(buf.writableSize() == 65535 - 4 - 4 - s1.length() - s2.length());
  buf.forceShrink();
  assert(buf.writableSize() == 65535 - 4 - s2.length());

  s = buf.readString();
  assert(s == s2);
  buf.forceShrink();
  assert(buf.writableSize() == 65535);
  assert(buf.readableSize() == 0);

  return 0;
}
