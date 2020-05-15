#include <string.h>
#include <stdio.h>
#include "concurrent_queue.h"

#define XSTR(x) #x

void lhandler(int b) {
  printf("%d\n", b);
}

#define STR(x, b) l##x (b)

int main() {
  int a = 1;
  int b =__sync_fetch_and_add(&a, 2);
  int c =__sync_add_and_fetch(&a, 2);
  printf("%d %d %d\n", a, b, c);

  STR(handler, 10);

  printf("%s", XSTR(11111));
  printf("%s\n", "xxx");
  tbb::concurrent_bounded_queue<int> q;
  q.push(100);
  printf("%uld\n", q.size());
  printf("%uld\n", q.capacity());
  printf("%uld\n", q.size());
  printf("%d\n", sizeof(std::ptrdiff_t));
}
