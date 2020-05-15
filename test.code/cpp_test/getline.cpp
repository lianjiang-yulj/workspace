#include <iostream>
#include <string>
#include <fstream>


int main()
{
    std::ifstream inf("a.txt");
    std::cout << inf.is_open()<<std::endl;
    std::cout << !inf<<std::endl;

    std::string line;
    while(getline(inf, line))
    {
        std::cout << line<<std::endl;
    }
    inf.close();
    return 0;
}
