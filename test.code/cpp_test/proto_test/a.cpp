#include "test.pb.h"

int main()
{
  pb::ResultFormatCBanerPb pb;
  pb.set_code(2);
  printf("%s\n", pb.DebugString().data());
  return 0;
}
