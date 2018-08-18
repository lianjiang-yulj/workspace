#include <stdio.h>
#include "class.h"

void test(AAA *p) {
	BBB *b = dynamic_cast<BBB*>(p);
	if (!b) {
		printf("dynamic_cast failed!\n");
		return;
	}
	b->test();
}

