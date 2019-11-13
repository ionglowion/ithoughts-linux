// itep
// quickie enscriptifier for ithought
// requires GNU Enscript to work
// may one day include a GUI print options chooser
// but more likely will do a .iteprc or environment var

// version 0.1 by David O'Toole (dto@gnu.org)
// this code is free

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define PADDING (4096)

char *menu_title(void) 
{
	return "enscript-it";
}

char *get_info(void)
{
	return "Print entry to default printer using GNU Enscript.";
}

void module_action(int widget, char **body)
{
	char *text = *body;
	FILE *f;

	f = popen("enscript", "w");
	fputs(text, f);
	pclose(f);
}
