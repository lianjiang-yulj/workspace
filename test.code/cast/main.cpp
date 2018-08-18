#include <dlfcn.h>
#include <stdio.h>
#include "class.h"

typedef void (*test_f)(AAA*);
int main() {
	void *h = dlopen("libtest.so", RTLD_NOW|RTLD_GLOBAL);
	if (!h) {
		printf("dlopen err: %s\n", dlerror());
		return 1;
	}
	test_f f = (test_f)dlsym(h, "_Z4testP3AAA");
	if (!f) {
		printf("dlsym err: %s\n", dlerror());
		return 2;
	}
	BBB b;
	f(&b);
	return 0;
}
