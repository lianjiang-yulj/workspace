#include "compare.h"

bool RealEcpmCompare::operator()(s& l, s& r)
{
    return l.b > r.b;
}
