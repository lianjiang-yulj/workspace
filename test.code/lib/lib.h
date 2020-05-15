#ifndef A_H
#define A_H

#include <stdio.h>
#include <vector>

class B {
public:
  B(){printf("B(%p)\n", this); p = new char[10];}
  virtual ~B(){printf("~B(%p)\n", this); delete p;}
  char* p;

};

class A {
 public:
  A(){printf("A(%p)\n", this);}
  virtual ~A(){printf("~A(%p)\n", this);}
  static void set(int i) {
    printf("b: %p\n", &b);
  }

  void seta(int i) {
    printf("bb: %p\n", &b);
  }
  static B b;
};


#endif
