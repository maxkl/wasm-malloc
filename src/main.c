
#ifdef __emscripten__
#include <emscripten.h>
#endif

#include "print.h"
#include "mm.h"

void test() {
	char *str = malloc(sizeof(*str) * 10);
	prints("malloc returned ");
	printptr(str);
	printc('\n');
}
