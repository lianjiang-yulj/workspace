#include <queue>
#include <vector>
#include <iostream>
#include <deque>

using namespace std;

struct cmp
{
    bool operator()(const int& a, const int& b)
    {
        return a > b;
    }
};
int main()
{
     int myints[]= {10,60,50,20,100};
     std::priority_queue<int, std::vector<int>, cmp> q;
    for (int i = 0; i < 5; i++)
    {
        q.push(myints[i]);
        if (q.size() >2) {
            cout << " pop: " << q.top();
            q.pop();
        }
    }

    cout <<endl;
    for (;!q.empty();) {
        cout << q.top() << endl;
        q.pop();
    }

    std::vector<int> v;
    v.resize(10);
    for (int i = 0; i < v.size(); i++) cout << v[i] << ',';
    v.resize(12);
    cout << endl;
    for (int i = 0; i < v.size(); i++) cout << v[i] << ',';
    cout << endl;
    return 0;
}

