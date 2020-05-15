#include <time.h>
#include <stdio.h>
#include <iostream>

using namespace std;

class A
{
    public:
        A(){}
        virtual ~A(){}
        A* get(){
            return NULL;
        }
        int q;
    static A a;
};
int main (int argc, char *argv[])
{
    struct tm* tmp_time = (struct tm*)malloc(sizeof(struct tm));
    strptime("Thu Jan 17 10:30:16 2013","%c",tmp_time);
    time_t t = mktime(tmp_time);
    cout << t << endl;
    cout << ctime(&t)<<endl;
    A a;
    A b;
    A* pa = a.get();
    A* pb = b.get();
    cout << pa << endl << pb<<endl;
    return 0;
}
