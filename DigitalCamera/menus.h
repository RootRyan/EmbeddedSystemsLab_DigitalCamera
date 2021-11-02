#include <stdint.h>

struct Menu{
	//keeps track of current state
		int cur;
	// array of all menus and sub menus 
		char** menu;
	//current selected value for each menu, refers to index in sub meny
		int* vals;
} mainmenu;

struct Menu Menu_Init(void);
