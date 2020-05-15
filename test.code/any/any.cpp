#include <iostream>
#include <string>
#include <queue>

using namespace std;
struct any_pointer
{
public:
    any_pointer()
    {
    }
    char * ptr;
};
struct any_accessor
{
    any_accessor(int a = 0):off(a){}
    template <typename T>
    inline T* getpointer(const any_pointer & p) const
    {
        return reinterpret_cast<T *>(p.ptr + off);
    }
    int32_t off;
};

template <typename T>
struct new_any:public any_pointer
{
public:
    explicit new_any(T a):b(a){ptr = (char *)&b;}
    T b;
};
template<typename T>
void f(any_pointer any,any_accessor acc)
{
    printf("%s\n",*acc.getpointer<T>(any));
}
int main()
{
    //priority_queue<int,base<int> > pq;
    any_accessor acc;

    new_any<int> newany = new_any<int>(11);
    int *p = acc.getpointer<int>(newany);
    printf("int = %d\n",*p);
   // f<uint32_t>(newany,acc);

    new_any<uint32_t> newanyu = new_any<uint32_t>(-1);
    uint32_t *pu = acc.getpointer<uint32_t>(newanyu);
    printf("uint = %u\n",*pu);
    //f<uint32_t>(newanyu,acc);

    new_any<double> newany2 = new_any<double>(43.12);
    double *pd = acc.getpointer<double>(newany2);
    printf("double = %lf\n",*pd);
    //f<double>(newany2,acc);
    char ppp[6]="12345";
    new_any<char *> newany3 = new_any<char *>(ppp);
    char * *ps = acc.getpointer<char *>(newany3);
    printf("char * = %s\n",*ps);
    f<char *>(newany3,acc);
    return 0;    
}
