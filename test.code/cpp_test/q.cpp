#include <queue> 
#include <deque> 
#include <iostream> 
    struct node { int value; };
    struct cmp { 
        bool operator()( const node& a, const node& b ) { return a.value < b.value; } 
    }; 
int main() { 
    std::priority_queue<node, std::deque<node>, cmp > pri_queue(); 
    node n1 = {22}; 
    node n2 = {11}; 
    node n3 = {33};
    node n4 = {11}; 
    pri_queue.push( n1); 
    pri_queue.push( n2);
    pri_queue.push( n3);
    pri_queue.push( n4); 
    while( ! pri_queue.empty() ) {
        node n = pri_queue.top();
        pri_queue.pop();
        std::cout << n.value << std::endl;
    } 
    return 0; 
};
