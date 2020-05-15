#include <iostream>
#include <map>
#include <vector>
#include <time.h>
#include <stdio.h>
using namespace std;
inline double isquare(double a)
{   
    return a*a;
}
double square(double a)
{   
    return a*a;
}
int main(){
    char * pszFieldValue="1330422976";
    time_t tmtTime = (time_t)atoi(pszFieldValue);
    struct tm tim;
    if(gmtime_r(&tmtTime, &tim) != NULL)
    {
        char pszResult[64];
        if(strftime(pszResult, 64, "%Y年%m月%d日 %H时%M分%S秒", &tim) < 0)
        {
            sprintf(pszResult, "%s", pszFieldValue);
        }
        cout << pszResult<<endl;
    }
    double a = 4;
    square(a);
    isquare(a);
    std::vector<int> vv;
    vv.push_back(10);
    map<std::string,std::string>mm;
    mm["guang"]="bin";
    mm["bin"]="222";
    std::string str = "aaac";
    cout << *str.begin() <<endl;
    cout << *(str.end()-1) <<endl;
    char *p = strdup(str.c_str());
    cout << p << endl;
    cout << "'/'" <<endl;
    delete p;
    
    return 0;
}
