#include <iostream>

#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

int main()
{
    struct in_addr in;
    char* ipstr = "10.20.162.127";
    inet_aton(ipstr, &in);
    uint32_t ipnum = ntohl(in.s_addr);
    cout << "ipnum: " << ipnum << endl;
    char* pstr = inet_ntoa(in);
    cout << "ipstr: " << pstr <<endl;
    in.s_addr = htonl(241605632);
    cout << "netlog: " << in.s_addr <<endl;
    pstr = inet_ntoa(in);
    cout << "ipstr: " << pstr <<endl;
    in.s_addr = htonl(241606655);
    cout << "netlog: " << in.s_addr <<endl;
    pstr = inet_ntoa(in);
    cout << "ipstr: " << pstr <<endl;
    return 0;
}
