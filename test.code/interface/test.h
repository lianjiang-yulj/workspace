#include <stdio.h>
typedef const char *(*func)();
#define DECLARE(function,version) \
extern "C" \
{ \
    const char * module##function##interface() \
    {\
        return version; \
    }\
    int a = 10;\
    int fadd(int x,int y); \
    extern const func finterface= module##function##interface; \
}

