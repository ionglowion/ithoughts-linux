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

#ifndef ITHOUGHT_ITHOUGHT_H
#define ITHOUGHT_ITHOUGHT_H

#include <gtk/gtk.h>

#include "action.h"
#include "callback.h"
#include "entry.h"
#include "file.h"
#include "ui.h"

/**
	Globals
**/

gchar	*gXmlFile;

gchar	*gActionText;

GSList	*gEntriesList;

GSList	*gModuleActionsList;
GSList	*gScriptActionsList;

Entry	*gCurrentEntry;

/**
	global widgets
**/

/* main window */
GtkWidget *main_window;
GtkWidget *main_notebook;
GtkWidget *main_file_menu;

/* entries tab */
GtkWidget *entries_tab_search_term;
GtkWidget *entries_tab_count_lbl;
GtkWidget *entries_tab_clist;
GtkWidget *entries_tab_entry_title;
GtkWidget *entries_tab_delete_btn;
GtkWidget *entries_tab_text;

/* actions tab */
GtkWidget *actions_tab_modules_clist;
GtkWidget *actions_tab_scripts_clist;
GtkWidget *actions_tab_module_info_lbl;

/* actions popup menu */
GtkWidget *actions_popup_menu;
GtkWidget *actions_popup_entry_title;
#endif
