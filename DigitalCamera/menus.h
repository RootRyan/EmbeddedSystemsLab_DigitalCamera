#include <stdint.h>

struct Menu{
		int cur;
		char** menu;
		char* vals;
} mainmenu;

struct Menu Menu_Init(void);
