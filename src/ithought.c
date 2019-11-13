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

#include <gtk/gtk.h>
#include <signal.h>

#include "ithought.h"
#include "callback.h"

int
main (int argc, char *argv[])
{
	gtk_init (&argc, &argv);

	gActionText			= NULL;
	gEntriesList		= NULL;
	gModuleActionsList	= NULL;
	gScriptActionsList	= NULL;
	gCurrentEntry		= NULL;

	ithought_file_init();
	ithought_action_init();
	ithought_ui_init();
	ithought_entry_init();

	signal (SIGINT, (void *)ithought_callback_close);

	gtk_main();

	return (-1);
}
