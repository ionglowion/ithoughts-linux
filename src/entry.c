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

#include "ithought.h"

/* static prototypes */
static void ithought_entry_update_mod (Entry *);

/* functions */

void
ithought_entry_init (void)
{
	gchar *path;

	path = g_strdup_printf ("%s/.ithought/ithought_db.xml", g_getenv ("HOME"));
	gEntriesList = ithought_file_load (path);
	g_free (path);

	ithought_entry_make_current (NULL);
	ithought_ui_entries_clist_populate();
}

void
ithought_entry_new (gchar *name, gchar *text)
{
	Entry	*entry;
	GDate	*date = g_date_new();
	gchar	*tmp;

	ithought_entry_save_current();

	entry = g_malloc (sizeof (Entry));
	g_date_set_time (date, time (NULL));
	tmp = g_malloc (sizeof (gchar *));
	g_date_strftime (tmp, 11, "%Y.%m.%d", date);

	entry->name = g_strdup (name);
	entry->mod  = g_strdup (tmp);
	entry->date = g_strdup (tmp);
	entry->text = g_strdup (text);

	g_date_free (date);
	g_free (tmp);

	gEntriesList = g_slist_append (gEntriesList, entry);

	ithought_entry_make_current (entry);

	ithought_ui_entries_clist_populate();
	ithought_ui_entries_tab_refresh();
}

void
ithought_entry_save_current (void)
{
	gchar *tmp;

	if (!gCurrentEntry)
		return;

	tmp = gtk_editable_get_chars (GTK_EDITABLE (entries_tab_text), 0, -1);

	if (strcmp (gCurrentEntry->text, tmp)) {
		g_free (gCurrentEntry->text);
		gCurrentEntry->text = g_strdup (tmp);
		ithought_entry_update_mod (gCurrentEntry);
	}
	g_free (tmp);
}

void
ithought_entry_make_current (Entry *entry)
{
	gCurrentEntry = entry;
	ithought_ui_entries_tab_refresh();
}

void
ithought_entry_rename (Entry *entry, gchar *new_name)
{
	gint row;

	g_free (entry->name);
	entry->name = g_strdup (new_name);

	row = gtk_clist_find_row_from_data (GTK_CLIST (entries_tab_clist), entry);
	if (row >= 0){
		gtk_clist_set_text (GTK_CLIST (entries_tab_clist), row, 0, entry->name);
		ithought_ui_entries_clist_show_row (row);
	}
}

void
ithought_entry_delete (Entry *entry)
{
	if (!entry)
		return;
	if (entry == gCurrentEntry)
		gCurrentEntry = NULL;

	gEntriesList = g_slist_remove (gEntriesList, entry);

	g_free (entry);
	ithought_ui_entries_clist_populate();
	ithought_ui_entries_tab_refresh();
}

/* static functions */

static void
ithought_entry_update_mod (Entry *entry)
{
	GDate *date = g_date_new();
	gint row;

	g_date_set_time (date, time (NULL));
	g_date_strftime (entry->mod, 11, "%Y.%m.%d", date);
	g_date_free (date);

	row = gtk_clist_find_row_from_data (GTK_CLIST (entries_tab_clist), entry);
	if (row >= 0)
		gtk_clist_set_text (GTK_CLIST (entries_tab_clist), row, 2, entry->mod);
}
