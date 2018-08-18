#include <string>
#include <iostream>

#include <stdio.h>

#define INT_DIGITS 19        /* enough for 64 bit integer */
char *itoa(int i)
{
    static char buf[INT_DIGITS + 2];
    buf[INT_DIGITS + 1] = '\0';
    char *p = buf + INT_DIGITS + 1;
    if (i >= 0) {
        do {
            *--p = '0' + (i % 10);
            i /= 10;
        } while (i != 0);
        return p;
    }
    else {  
        do {
            *--p = '0' - (i % 10);
            i /= 10;
        } while (i != 0);
        *--p = '-';
    }
    return p;
}

int main()
{
    std::string a;
    for (int i = 0; i < 1000000; i++)
    {
        a.append(itoa(i));
        printf("%s\n",itoa(i));
    }

    return 0;
}
