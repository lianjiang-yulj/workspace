#include <iostream>
#include <dlfcn.h>
using namespace std;
int main(){
	void *handle = dlopen( "/home/yulj/Test/dl_so_test/lib/libfile.so", RTLD_LAZY );
	if (NULL == handle) cout << "dlopen failure " << endl;
	else 
	{
		typedef void (*fun)();
		fun funp = (fun ) dlsym(handle,"ffa");
		const char* dlsym_error=dlerror();

		if(NULL != dlsym_error) {
		   dlclose(handle);
		   cout << "dlsym failure" << endl;
  		   return 1;
 		}

		cout << "dlsym success" << endl;
        	cout << "test" << endl;
		funp();
		funp = (fun ) dlsym(handle,"ffb");
		dlsym_error=dlerror();
		
                if(NULL != dlsym_error) {
                   dlclose(handle);
                   cout << "dlsym failure" << endl;
                   return 1;
                }
		funp();
 		dlclose(handle);
	}
	return 0;
}
