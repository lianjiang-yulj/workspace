
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <iostream>
#include "PriorityQueue.h"

using namespace std;

struct node
{
    int a, b;
};
struct cmp1
{
    bool operator()(node& a , node& b){ 
        if (a.a != b.a)return a.a < b.a;
        return a.b < b.b;
    }
};

bool func(node& a, node&b) {
    return a.a < b.a;
}

struct cmp2
{
    bool operator()(node& a , node& b){ 
        if (a.a != b.a)return a.a > b.a;
        return a.b > b.b;
    }
};


typedef queue_utility::CPriorityQueue<node, cmp1> myq1;
typedef queue_utility::CPriorityQueue<node, cmp2> myq2;

int main(){

    myq1 q1(4);
    myq2 q2(4);
    
    node n;
    n.a = 1;
    n.b = 1;
    q1.insert(n);
    q2.insert(n);

    n.a = 2;
    n.b = 3;
    q1.insert(n);
    q2.insert(n);

    n.a = 2;
    n.b = 4;
    q1.insert(n);
    q2.insert(n);
    n.a = 2;
    n.b = 5;
    q1.insert(n);
    q2.insert(n);
    n.a = 2;
    n.b = 6;
    q1.insert(n);
    q2.insert(n);

    n.a = 3;
    n.b = 3;
    q1.insert(n);
    q2.insert(n);

    n.a = 4;
    n.b = 3;
    q1.insert(n);
    q2.insert(n);

    n.a = 5;
    n.b = 3;
    q1.insert(n);
    q2.insert(n);

    n.a = 5;
    n.b = 9;
    q1.insert(n);
    q2.insert(n);




    int size = q1.size();
    cout << "q1 up"<<endl;
    for (int j = 0; j < size; j++)
    {
        cout << q1.top().a << ' ' << q1.top().b<<endl;
        q1.pop();
    }
    
    cout << "q2 down"<<endl;
    for (int j = 0; j < size; j++)
    {
        cout << q2.top().a << ' ' << q2.top().b<<endl;
        q2.pop();
    }

    return 0;
}
