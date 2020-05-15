#include <google/dense_hash_map>
#include <iostream>
#include <string>
#include <map>

using namespace std;
using google::dense_hash_map;      // namespace where class lives by default

struct eqstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    if (s1 && s2)
    return strcmp(s1, s2) <= 0;
    else if (s1) return 0;
    else return 1;
  }
};

int main()
{
  dense_hash_map<const char*, int, SPARSEHASH_HASH<const char*> > months;
  //dense_hash_map<const char*, int, SPARSEHASH_HASH<const char*>, eqstr > months;

  // must set empty key
  months.set_empty_key(NULL);
  months["january"] = 31;
  months["february"] = 28;
  months["december"] = 31;
  // 且不能使用empty key
 // months[NULL] = 32;

  cout << (months.find("september") == months.end()) << endl;
  cout << "september -> " << months["september"] << endl;
  cout << (months.find("september") == months.end()) << endl;
  cout << "april     -> " << months["april"] << endl;
  cout << "june      -> " << months["june"] << endl;
  cout << "november  -> " << months["november"] << endl;
  std::string v = "\"\'<>&";
  cout << v << endl;
  char* p;
  if (p = strpbrk(v.data(), "\"\'<>&")) {
    cout << p << " hit" << endl;
  }

  std::map<int , int> imap;
  imap[5] = 10;
  return 0;
}
