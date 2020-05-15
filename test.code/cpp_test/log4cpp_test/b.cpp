#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

int main(int argc, char* argv[])
{
  std::string initFileName = "log4cpp.properties";
  log4cpp::PropertyConfigurator::configure(initFileName);

  log4cpp::Category& root = log4cpp::Category::getRoot();

  log4cpp::Category& sub1 = 
      log4cpp::Category::getInstance(std::string("sub1"));

  log4cpp::Category& sub2 = 
      log4cpp::Category::getInstance(std::string("sub1.sub2"));

  log4cpp::Category& cban = 
      log4cpp::Category::getInstance(std::string("cban"));

  root.warn("warn is coming");
  root.info("info for storm.");
  
  sub1.debug("debug storm warning");
  sub1.info("info all hatches");
  sub1.warn("warn");
  sub1.fatal("fatal");

  sub2.debug("debug solar panels");
  sub2.info("info panels are shielded");
  sub2.error("error panels are blocked");
  sub2.warn("Unfolding protective shield");
  sub2.fatal("fatal protective shield");

  char cha[10] = {0};
  cha[0] = 'a';
  cha[1] = 'b';
  cha[2] = '\0';
  cha[3] = 'c';
  printf("cha = %s\n", cha);         

  std::string msg(cha, 4);
  printf("msg =[%s], len=%d\n", msg.data(), msg.size());

  cban.info("fatal protective shield");
  cban.info(msg.data());
  cban.info("fatal protective shield");


  log4cpp::Category::shutdown();

  return 0;
}
