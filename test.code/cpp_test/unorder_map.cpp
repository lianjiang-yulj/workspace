#include <string>

#include <iostream>

#include <algorithm>

#include <tr1/unordered_map>
#include <unordered_map>

using namespace std;
using namespace tr1;
using namespace std::tr1;

void myfunc(const std::pair<std::string,std::string>& obj)

{

  std::cout << "first=" << obj.first << "second=" << obj.second <<std::endl;

}



int main(int argc,char *argv[])

{

  std::tr1::unordered_map<std::string, std::string> ssmap;



  ssmap.insert(std::make_pair("aaaa","aaaaa1"));

  ssmap.insert(std::make_pair("bbbb","bbbbb1"));

  ssmap.insert(std::make_pair("cccc","ccccc1"));



  std::for_each(ssmap.begin(),ssmap.end(),myfunc);



  std::tr1::unordered_map<std::string, std::string>::iterator iter = ssmap.begin();
  for(;iter != ssmap.end();iter++){

    std::cout << "first=" << iter->first << "second=" << iter->second <<std::endl;

  }

  return 0;
}
