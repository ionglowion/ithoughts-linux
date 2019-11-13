/*
	ithought - an extensible text management system
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
#ifndef ITHOUGHT_CALLBACK_H
#define ITHOUGHT_CALLBACK_H

#include <gtk/gtk.h>

void ithought_callback_close				(void);
void ithought_callback_entries_sort			(void);
void ithought_callback_entries_clist_click	(GtkWidget *, GdkEventButton *,
                                             gpointer);
void ithought_callback_entries_clist_sel	(GtkWidget *, gint, gint,
                                             GdkEventButton *, gpointer);
void ithought_callback_entries_clist_unsel	(GtkWidget *, gint, gint,
                                             GdkEventButton *, gpointer);
void ithought_callback_actions_clist_sel	(GtkWidget *, gint, gint, 
                                             GdkEventButton *, gpointer);
void ithought_callback_actions_clist_unsel	(GtkWidget *, gint, gint, 
                                             GdkEventButton *, gpointer);
void ithought_callback_rename_kpress		(void);
void ithought_callback_delete_btn			(void);
void ithought_callback_new_btn				(void);
#endif
