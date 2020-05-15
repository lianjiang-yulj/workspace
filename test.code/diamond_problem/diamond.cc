#include <iostream>
#include <cstdio>
#include <cstdint>

using namespace std;

class Animal {
 public:
  Animal() : weight_(0) {}
  virtual int32_t weight() {
    cout << " Animal: " << weight_ << ""  <<  __LINE__ << endl;
  }
  virtual ~Animal() { cout << "destruct~" << endl;}

  int32_t weight_;
};

class Tiger : virtual public Animal
{
 public:
  virtual ~Tiger(){ cout << "Tiger destruct" << endl;}
};

class Lion : virtual public Animal
{
 public:
  virtual ~Lion(){ cout << "Lion destruct" << endl;}
};

class Liger : public Tiger, public Lion { /* ... */ };

int32_t main() {

 Liger lg;
 //lg.weight();
  cout << __cplusplus << endl;

  Animal* animal = new Liger();
  delete animal;
  return 0;
}
