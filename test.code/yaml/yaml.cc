#include "yaml-cpp/yaml.h"

int main ()
{
  YAML::Node testnode;
  printf("%d\n", testnode.Type());

  YAML::Node node = YAML::LoadFile("test.yaml");
  printf("%d\n", node.Type());
  printf("%d\n", node["key1"].Type());
  printf("%d\n", node["key2"].Type());
  printf("%s\n", node["key2"].as<std::string>().data());
  printf("%d\n", node["key3"].Type());
  printf("%d\n", node["key4"].Type());
  printf("%d\n", node["key5"].Type());
  YAML::Node node4 = node["key4"];
  printf("%d %d\n", node4.Type(), node4.size());
  for(YAML::const_iterator iter=node4.begin(); iter != node4.end(); ++iter) {
    printf("%d\n", (*iter)["k1"].Type());
    printf("%d\n", (*iter)["v1"].Type());

  }
  YAML::Node node5 = node["key5"];
  printf("%d %d\n", node5.Type(), node5.size());
  for(YAML::const_iterator iter=node5.begin(); iter != node5.end(); ++iter) {
    printf("%s\n", (*iter).as<std::string>().data());

  }

  YAML::Node node6 = node["key6"];
  printf("%d %d\n", node6.Type(), node6.size());
  for(YAML::const_iterator iter=node6.begin(); iter != node6.end(); ++iter) {
    const YAML::Node& key = iter->first;
    const YAML::Node& buckets = iter->second;
    printf("%s\n", key.as<std::string>().data());
    printf("%s\n", buckets.as<std::string>().data());

  }


  YAML::Node node1;  // starts out as null
  YAML::Node nodem;  // starts out as null
  node["bucket"] = "b";  // it now is a map node
  node["weight"] = "w";  // it now is a map node
  node["strategy"] = "s";  // it now is a map node
  nodem["seq"].push_back(node1);  // node["seq"] automatically becomes a sequence
  nodem["seq"].push_back(node1);

  printf("nodem:%d\n", nodem.IsMap());
  printf("nodem:%d\n", nodem["seq"].IsSequence());
  return 0;
}

