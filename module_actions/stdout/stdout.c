#include <stdio.h>

/*
This is an example ithought action module that outputs the body of an entry
to stdout using printf()
*/
char *menu_title( void ){
	return( "stdout" );
}

char *get_info( void ){
	return( "The stdout module outputs the entry body to stdout." );
}

void module_action( int widget, char **body ){
	printf( "%s\n", *body );
}
