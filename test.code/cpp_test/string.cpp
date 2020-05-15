#include <string>

#include <stdio.h>

using namespace std;

int main()
{
    std::string str;
    printf("%s %d\n", str.c_str(), str.length());
    printf("%s %d\n", str.data(), str.length());
    return 0;
}

