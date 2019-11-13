/*
	livejournal.c - ithought action plugin
	Copyright (C) 2001 Anthony Taranto <voltronw@yahoo.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>

#include <gtk/gtk.h>

typedef struct lj_instance {
	char *ithought_text;

	GtkWidget *window;

	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *table;
	
	GtkWidget *username_lbl;
	GtkWidget *password_lbl;

	GtkWidget *username;
	GtkWidget *password;

	GtkWidget *separator;

	GtkWidget *subject_lbl;
	GtkWidget *subject;

	GtkWidget *scroller;
	GtkWidget *entry;

	GtkWidget *status;

	GtkWidget *cancel;
	GtkWidget *submit;
} lj_instance;

char *menu_title( void ){
	return( "livejournal" );
}

char *get_info( void ){
	return( "The livejournal action uploads an entry to your diary on http://www.livejournal.com." );
}

static void set_status( char *text, lj_instance *i ){
	gtk_label_set_text( GTK_LABEL( i->status ), text );
	while( gtk_events_pending() )
		gtk_main_iteration();
}

static void window_close_cb( GtkWidget *widget, lj_instance *i ){
	if( GTK_IS_WIDGET( i->window ) )
		gtk_widget_destroy( GTK_WIDGET( i->window ) );
	else gtk_widget_destroy( GTK_WIDGET( widget ) );
}

static gchar *conv_to_hex( gchar x, gchar *hex )
{
	hex[0] = "0123456789ABCDEF"[(int) x / 16];
	hex[1] = "0123456789ABCDEF"[(int) x % 16];
	hex[2] = '\0';
	
	return hex;
}

static gchar *url_encode( gchar *entry )
{
	gchar *hex;
	GString *buf;
	gchar *p;

	hex = (gchar *) g_malloc(3);
	buf = g_string_sized_new( strlen(entry) );
	p = entry;
	
	while (*p != '\0') {
		/* If printable, append to the end of the new string. */
		if ( ((*p >= 'A') && (*p <= 'Z')) ||
		     ((*p >= 'a') && (*p <= 'z')) ||
		     ((*p >= '0') && (*p <= '9'))    ) {
			g_string_append_c( buf, *p );
		} else if (*p == ' ') {
			g_string_append_c( buf, '+' );
		} else {
			g_string_append_c( buf, '%' );
			
			hex = conv_to_hex( *p, hex );
			g_string_append( buf, hex );
		}
		
		p++;
	}

	p = g_strdup( buf->str );
	g_string_free( buf, TRUE );

	g_free( hex );
	
	return p;
}

/*XXX: this function is a mess, it probably leaks memory like crazy*/
static void submit_cb( GtkWidget *widget, lj_instance *i ){
	int s;
	char buf[1024];
	int bytes_sent, bytes_read, len;
	struct hostent *hp;
	struct sockaddr_in address;
	char *command, *post_string, *username, *password, *entry, *subject;
	char *date_string, *time_string;
	GDate *date = g_date_new();
	char hostname[] = "www.livejournal.com";

	subject = time_string = post_string = command = entry = username = password
		= NULL;
	
	if ((hp = gethostbyname (hostname)) == NULL) {
		set_status( "Unable to connect to www.livejournal.com"
			": Unknown host", i );
		goto failure;
	}
	if ((s = socket( AF_INET, SOCK_STREAM, 0 )) == -1) {
		set_status( "Socket init error.", i );
		goto failure;
	}

	address.sin_family = AF_INET;
	address.sin_port   = htons( 80 );
	bcopy( hp->h_addr, &address.sin_addr, hp->h_length );

	set_status( "Connecting...", i );

	if( connect( s, (struct sockaddr *)&address, sizeof( struct sockaddr ) ) ){
		set_status( "Connection to livejournal failed!", i );
		goto failure;
	}

	set_status( "Connected", i );

	username = gtk_editable_get_chars( GTK_EDITABLE( i->username ), 0, -1 );
	password = gtk_editable_get_chars( GTK_EDITABLE( i->password ), 0, -1 );
	subject  = gtk_editable_get_chars( GTK_EDITABLE( i->subject  ), 0, -1 );

	password = url_encode( password );

	post_string = g_strdup_printf( "mode=login&user=%s&password=%s"
		"&clientversion=Gtk-ithought_livejournal",
		username, password );

	command = g_strdup_printf( "POST /cgi-bin/log.cgi HTTP/1.0\r\n"
					"Host: www.livejournal.com\r\n"
					"Content-type: multipart/form-data\r\n"
					"Content-length: %d\n\n%s",
					strlen( post_string ), post_string );

	set_status( "Logging In...", i );
	
	len = strlen( command );
	bytes_sent = send( s, command, len, 0 );

	if( bytes_sent < len ){
		set_status( "Login Failed: did not send full command", i );
		goto failure;
	}

	g_free( command );

	/*now, check and see if our login was accepted: */
	bytes_read = recv( s, buf, 1023, 0 );
	if( bytes_read < 0 ){
		printf( "recv() returned -1!!\n" );
		exit( -1 );
	}
	buf[bytes_read] = '\0';

	if( !strstr( buf, "success\nOK" ) ){
		set_status( "Login Failed!", i );
		goto failure;
	}

	/*XXX: this really should parse the response in to a hash the way it was
		designed to be. that would let uss give better error messages */
	
	set_status( "Login OK", i );

	/*reopen the socket*/
	close( s );
	s = socket( AF_INET, SOCK_STREAM, 0 );
	if( connect( s, (struct sockaddr *)&address, sizeof( struct sockaddr ) ) ){
		set_status( "Connection to livejournal failed!", i );
		goto failure;
	}

	/*now upload our diary entry*/
	//set_status( "Uploading Diary...", i );

	entry = gtk_editable_get_chars( GTK_EDITABLE( i->entry ), 0, -1 );
	
	g_date_set_time( date, time( NULL ) );
	date_string = g_malloc( sizeof( gchar * ) );
	g_date_strftime( date_string, 39, "year=%Y&mon=%m&day=%d",
		date );
	g_date_free( date );

	{ /* set hour and minute, becuase glib doesn't */
		time_t ct = time( NULL );
		struct tm *curtime = localtime( &ct );

		time_string = g_strdup_printf( "hour=%d&min=%d",
			curtime->tm_hour, curtime->tm_min );
	}

	entry = url_encode( entry );

	post_string = g_strdup_printf( "mode=postevent&user=%s&password=%s"
		"&%s&%s&event=%s&subject=%s",
		username, password, date_string, time_string, entry, subject);

	command = g_strdup_printf( "POST /cgi-bin/log.cgi HTTP/1.0\r\n"
					"Host: www.livejournal.com\r\n"
					"Content-type: multipart/form-data\r\n"
					"Content-length: %d\n\n%s",
					strlen( post_string ), post_string );

	len = strlen( command );
	bytes_sent = send( s, command, len, 0 );

	if( bytes_sent < len ){
		set_status( "Diary Upload Failed: did not send full command", i );
		goto failure;
	}

	bytes_read = recv( s, buf, 1023, 0 );
	if( bytes_read < 0 ){
		printf( "recv() returned -1!!\n" );
		exit( -1 );
	}
	buf[bytes_read] = '\0';

	set_status( "Diary Uploaded Successfully", i );

failure:
	close( s );
	
	/* g_free( command ); this was causing a 'chunk is already free' warning */
	g_free( entry );
	g_free( username );
	g_free( password );
	g_free( post_string );
	g_free( time_string );
	g_free( subject );
}

void module_action( int widget, char **body ){
	lj_instance *i = g_malloc( sizeof( lj_instance ) );
	i->ithought_text = g_strdup( *body );

	i->window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_widget_set_usize( GTK_WIDGET( i->window ), 300, 200 );
	gtk_window_set_title( GTK_WINDOW( i->window ),
		"ithought - livejournal action");
	gtk_container_set_border_width( GTK_CONTAINER( i->window ), 5 );
	gtk_signal_connect( GTK_OBJECT( i->window ), "delete_event",
		GTK_SIGNAL_FUNC( window_close_cb ), i );

	i->vbox = gtk_vbox_new( FALSE, 5 );
	
	i->table = gtk_table_new( 2, 2, FALSE );
	gtk_box_pack_start( GTK_BOX( i->vbox ), i->table, FALSE, FALSE, 0 );

	i->username_lbl = gtk_label_new( "Username:" );
	gtk_table_attach_defaults( GTK_TABLE( i->table ), i->username_lbl,
		0, 1, 0, 1 );

	i->username = gtk_entry_new();
	gtk_table_attach_defaults( GTK_TABLE( i->table ), i->username,
		1, 2, 0, 1 );

	i->password_lbl = gtk_label_new( "Password:" );
	gtk_table_attach_defaults( GTK_TABLE( i->table ), i->password_lbl,
		0, 1, 1, 2 );

	i->password = gtk_entry_new();
	gtk_entry_set_visibility( GTK_ENTRY( i->password ), FALSE );
	gtk_table_attach_defaults( GTK_TABLE( i->table ), i->password,
		1, 2, 1, 2 );

	i->separator = gtk_hseparator_new();
	gtk_box_pack_start( GTK_BOX( i->vbox ), i->separator, FALSE, FALSE, 0 );

	i->hbox = gtk_hbox_new( FALSE, 5 );
	gtk_box_pack_start( GTK_BOX( i->vbox ), i->hbox, FALSE, FALSE, 0 );

	i->subject_lbl = gtk_label_new( "Subject:" );
	gtk_box_pack_start( GTK_BOX( i->hbox ), i->subject_lbl, FALSE, FALSE, 0 );

	i->subject = gtk_entry_new();
	gtk_box_pack_start( GTK_BOX( i->hbox ), i->subject, TRUE, TRUE, 0 );

	i->scroller = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( i->scroller ),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS );
	gtk_box_pack_start( GTK_BOX( i->vbox ), i->scroller, TRUE, TRUE, 0 );

	i->entry = gtk_text_new( NULL, NULL );
	gtk_text_freeze( GTK_TEXT( i->entry ) );
	gtk_text_set_point( GTK_TEXT( i->entry ), 0 );
	gtk_text_insert( GTK_TEXT( i->entry ), NULL, NULL, NULL,
		i->ithought_text, -1 );
	gtk_text_set_editable( GTK_TEXT( i->entry ), TRUE );
	gtk_text_thaw( GTK_TEXT( i->entry ) );
	gtk_container_add( GTK_CONTAINER( i->scroller ), i->entry );

	i->hbox = gtk_hbox_new( FALSE, 5 );
	gtk_box_pack_start( GTK_BOX( i->vbox ), i->hbox, FALSE, FALSE, 0 );

	i->status = gtk_label_new( "" );
	gtk_box_pack_start( GTK_BOX( i->hbox ), i->status, FALSE, FALSE, 0 );

	i->submit = gtk_button_new_with_label( "Submit" );
	gtk_signal_connect( GTK_OBJECT( i->submit ), "clicked",
		GTK_SIGNAL_FUNC( submit_cb ), i );
	gtk_box_pack_end( GTK_BOX( i->hbox ), i->submit, FALSE, FALSE, 0 );

	i->cancel = gtk_button_new_with_label( "Close" );
	gtk_signal_connect( GTK_OBJECT( i->cancel ), "clicked",
		GTK_SIGNAL_FUNC( window_close_cb ), i );
	gtk_box_pack_end( GTK_BOX( i->hbox ), i->cancel, FALSE, FALSE, 0 );

	gtk_container_add( GTK_CONTAINER( i->window ), i->vbox );

	gtk_widget_show_all( i->window );
}

