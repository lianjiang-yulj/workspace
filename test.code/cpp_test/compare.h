#include <functional>
#include <algorithm>

struct s
{
    int a;
    double b;
    int32_t r;
};

struct RealEcpmCompare 
{
public:
    RealEcpmCompare(int32_t* wtwpr) : m_wtwpr(wtwpr)
    {
    }

public:
    bool operator()(s& l, s& r);

private:
    int32_t* m_wtwpr;
};


