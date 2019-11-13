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

#ifndef ITHOUGHT_ACTION_H
#define ITHOUGHT_ACTION_H

#include <gtk/gtk.h>
#include <gmodule.h>

typedef struct _ModuleAction ModuleAction;
struct _ModuleAction {
	GModule		*module;

	GtkWidget	*menu_item;

	void		(*do_action)(gchar *);
	gchar		*(*get_info)(void);
	gchar		*(*menu_title)(void);
};

typedef struct _ScriptAction ScriptAction;
struct _ScriptAction {
	gchar		*file_path;

	GtkWidget	*menu_item;

	gchar		*title;
	gchar		*info;
};

void ithought_action_init		(void);
void ithought_action_do_script	(GtkWidget *, ScriptAction *);

#endif
