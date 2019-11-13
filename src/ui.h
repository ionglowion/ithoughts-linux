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

#ifndef ITHOUGHT_UI_H
#define ITHOUGHT_UI_H

void ithought_ui_init					(void);
void ithought_ui_entries_clist_populate	(void);
void ithought_ui_entries_clist_show_row	(gint);
void ithought_ui_entries_tab_refresh	(void);
void ithought_ui_actions_menu_refresh	(void);

#endif
