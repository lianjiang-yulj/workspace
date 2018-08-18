#include <stdio.h>
#include <string>
#include <stdlib.h>

int main()
{   
    std::string a;
    char arr[64];
    for (int i = 0; i < 1000000; i++)
    {
        snprintf(arr,sizeof(arr), "%d", i);
        a.append(arr);
    }

    return 0;
}   
