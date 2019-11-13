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

/* functions */

void
ithought_callback_close (void)
{
	ithought_entry_save_current();
	ithought_file_save();
	gtk_exit (0);
}

void
ithought_callback_entries_sort (void)
{

}

void
ithought_callback_entries_clist_click (GtkWidget *widget, GdkEventButton *event,
                                       gpointer data)
{
	gint	col, row = -1;
	Entry	*tmp;

	if (event->button != 3)
		return;

	gtk_clist_get_selection_info (GTK_CLIST (widget), event->x, event->y,
                                  &row, &col);
	if (row == -1)
		return;

	tmp = gtk_clist_get_row_data (GTK_CLIST (widget), row);

	g_free (gActionText);
	gActionText = NULL;

	if (gCurrentEntry == tmp)
		gActionText = gtk_editable_get_chars (GTK_EDITABLE ( entries_tab_text),
		                                      0, -1);
	else
		gActionText = g_strdup (tmp->text);

	gtk_widget_destroy (actions_popup_entry_title);
	actions_popup_entry_title = gtk_menu_item_new_with_label (tmp->name);
	gtk_menu_prepend (GTK_MENU (actions_popup_menu),
	                  actions_popup_entry_title);
	gtk_widget_set_sensitive (actions_popup_entry_title, FALSE);
	gtk_widget_show (actions_popup_entry_title);

	gtk_menu_popup (GTK_MENU (actions_popup_menu), NULL, NULL, NULL, NULL,
	                event->button, event->time);
}

void
ithought_callback_entries_clist_sel (GtkWidget *widget, gint row, gint col,
                                     GdkEventButton *event, gpointer data)
{
	Entry *entry = gtk_clist_get_row_data (GTK_CLIST (widget), row);
	ithought_entry_save_current();
	ithought_entry_make_current (entry);
}

void
ithought_callback_entries_clist_unsel (GtkWidget *widget, gint row, gint col,
                                       GdkEventButton *event, gpointer data)
{
	ithought_entry_save_current();
	ithought_entry_make_current (NULL);
}

void
ithought_callback_actions_clist_sel (GtkWidget *widget, gint row, gint col,
                                     GdkEventButton *event, gpointer data)
{
	gchar *string = NULL;

	if (widget == actions_tab_modules_clist) {
		ModuleAction *tmp;

		tmp = gtk_clist_get_row_data (GTK_CLIST (widget), row);
		string = g_strdup_printf ("Module File:\n%s\n\nModule Info:\n%s",
		                          g_module_name (tmp->module), tmp->get_info());
		gtk_label_set_text (GTK_LABEL (actions_tab_module_info_lbl), string);
	}
	if (widget == actions_tab_scripts_clist) {
		ScriptAction *tmp;

		tmp = gtk_clist_get_row_data (GTK_CLIST (widget), row);
		string = g_strdup_printf ("Script File:\n%s\n\nScript Info:\n%s",
		                          tmp->file_path, tmp->info);
		gtk_label_set_text (GTK_LABEL (actions_tab_module_info_lbl), string);
	}
	g_free (string);
	ithought_ui_actions_menu_refresh();
}

void
ithought_callback_actions_clist_unsel (GtkWidget *widget, gint row, gint col,
                                       GdkEventButton *event, gpointer data)
{
	if (widget == actions_tab_modules_clist) {

	}
	if (widget == actions_tab_scripts_clist) {

	}
	gtk_label_set_text (GTK_LABEL (actions_tab_module_info_lbl), "");

	ithought_ui_actions_menu_refresh();
}

void
ithought_callback_rename_kpress (void)
{
	gchar *tmp;

	if (!gCurrentEntry)
		return;

	tmp = gtk_editable_get_chars (GTK_EDITABLE (entries_tab_entry_title),
	                              0, -1);
	ithought_entry_rename (gCurrentEntry, tmp);
	g_free (tmp);
}

void
ithought_callback_delete_btn (void)
{
	if (!gCurrentEntry)
		return;

	ithought_entry_delete (gCurrentEntry);
}

void
ithought_callback_new_btn (void)
{
	ithought_entry_new ("new entry", "empty");
}
