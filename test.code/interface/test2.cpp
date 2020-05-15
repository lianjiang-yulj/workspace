#include "test.h"
#include <stdio.h>
using namespace std;
extern "C" int fadd(int x,int y);
extern "C" int a;
extern const func finterface;
int main()
{
    printf ("%d\n",fadd(1,2));
    printf ("%d\n",a);
    printf ("%s\n",finterface());
}
