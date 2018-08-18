#include <sstream>
#include <string>
using namespace std;

int main()
{
    ostringstream result(ostringstream::out);
    for (int i = 0; i < 1000000;i++)
    {
        //ostringstream result(ostringstream::out);
        result<<i;
    }
    std::string a = result.str();
    return 0;
}
