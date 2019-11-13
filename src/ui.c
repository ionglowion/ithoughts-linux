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
static void			 ithought_ui_create_main				(void);
static void			 ithought_ui_create_entries_tab			(void);
static void			 ithought_ui_create_actions_tab			(void);
static void			 ithought_ui_create_about_tab			(void);
static void			 ithought_ui_create_actions_menu		(void);
static GtkWidget	*ithought_ui_new_scroller				(void);

/* functions */

void
ithought_ui_init (void)
{
	ithought_ui_create_main();

	ithought_ui_create_entries_tab();
	ithought_ui_create_actions_tab();

	ithought_ui_create_actions_menu();
	
	gtk_widget_show_all (main_window);
}

void
ithought_ui_entries_clist_populate (void)
{
	Entry	*entry;
	GSList	*i;
	gchar	*data[3], *string, *number;
	gint	shown = 0, total = 0, moveto = 0;

	string = gtk_editable_get_chars (GTK_EDITABLE (entries_tab_search_term),
	                                 0, -1);

	ithought_entry_save_current();
	
	gtk_clist_freeze (GTK_CLIST (entries_tab_clist));
	gtk_clist_clear (GTK_CLIST (entries_tab_clist));
	
	for (i = gEntriesList; i; i = i->next) {
		entry = i->data;
		total++;
		if (strstr (entry->text, string)) {
			data[0] = entry->name;
			data[1] = entry->date;
			data[2] = entry->mod;
			gtk_clist_append (GTK_CLIST (entries_tab_clist), data);
			gtk_clist_set_row_data (GTK_CLIST (entries_tab_clist), shown,
			                        entry);
			if (entry == gCurrentEntry){
				gtk_clist_select_row( GTK_CLIST (entries_tab_clist), shown, 0);
				moveto = shown;
			}
			shown++;
		}
	}
	gtk_clist_thaw (GTK_CLIST (entries_tab_clist));
	
	ithought_ui_entries_clist_show_row (moveto);

	if (!strcmp (string, "") && shown < total)
		g_warning ("ithought: some entries are not being read correctly");

	number = g_strdup_printf ("%d / %d", shown, total);
	gtk_label_set_text (GTK_LABEL (entries_tab_count_lbl), number);

	g_free (number);
	g_free (string);
}

void
ithought_ui_entries_clist_show_row (gint row)
{
	if (gtk_clist_row_is_visible (GTK_CLIST (entries_tab_clist), row) !=
	    GTK_VISIBILITY_FULL)
	{
		gtk_clist_moveto (GTK_CLIST (entries_tab_clist), row, 0, 0, 0);
	}
}
void
ithought_ui_entries_tab_refresh (void)
{
	if (gCurrentEntry) {
		gtk_text_freeze (GTK_TEXT (entries_tab_text));
		gtk_text_set_point (GTK_TEXT (entries_tab_text), 0);
		gtk_text_forward_delete (GTK_TEXT (entries_tab_text),
		                         gtk_text_get_length (
								 GTK_TEXT (entries_tab_text)));
		gtk_text_insert (GTK_TEXT (entries_tab_text), NULL, NULL, NULL,
		                 gCurrentEntry->text, -1);
		gtk_text_set_editable (GTK_TEXT (entries_tab_text), TRUE);

		gtk_entry_set_text (GTK_ENTRY (entries_tab_entry_title),
		                    gCurrentEntry->name);
		gtk_text_thaw (GTK_TEXT (entries_tab_text));

		gtk_widget_set_sensitive (entries_tab_delete_btn, TRUE);
		gtk_editable_set_editable (GTK_EDITABLE (entries_tab_entry_title),
		                           TRUE);
	}
	else if (!gCurrentEntry) {
		gtk_text_freeze (GTK_TEXT (entries_tab_text));
		gtk_text_set_point (GTK_TEXT (entries_tab_text), 0);
		gtk_text_forward_delete (GTK_TEXT (entries_tab_text),
		                         gtk_text_get_length (
								 GTK_TEXT (entries_tab_text)));
		gtk_text_set_editable (GTK_TEXT (entries_tab_text), FALSE);
		gtk_entry_set_text (GTK_ENTRY (entries_tab_entry_title), "");
		gtk_text_thaw (GTK_TEXT (entries_tab_text));

		gtk_widget_set_sensitive (entries_tab_delete_btn, FALSE);
		gtk_editable_set_editable (GTK_EDITABLE (entries_tab_entry_title),
		                           FALSE);
	}
}

void
ithought_ui_actions_menu_refresh (void)
{
	ModuleAction	*module_action;
	ScriptAction	*script_action;
	GtkWidget		*clist;
	GList			*i;
	gint			row = 0;

	clist = actions_tab_modules_clist;
	while ((module_action = gtk_clist_get_row_data (GTK_CLIST (clist), row))) {
		if (g_list_find (GTK_CLIST (clist)->selection, GINT_TO_POINTER (row++)))
			gtk_widget_show (module_action->menu_item);
		else
			gtk_widget_hide (module_action->menu_item);
	}
	row = 0;
	clist = actions_tab_scripts_clist;
	while ((script_action = gtk_clist_get_row_data (GTK_CLIST (clist), row))) {
		if (g_list_find (GTK_CLIST (clist)->selection, GINT_TO_POINTER (row++)))
			gtk_widget_show (script_action->menu_item);
		else
			gtk_widget_hide (script_action->menu_item);
	}
}

/* static functions */

static void
ithought_ui_create_main (void)
{
	GtkWidget *vbox, *menubar, *generic;

	main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_usize (GTK_WIDGET (main_window), 600, 400);
	gtk_window_set_title (GTK_WINDOW (main_window), "ithought");
	gtk_signal_connect (GTK_OBJECT (main_window), "delete_event",
	                    GTK_SIGNAL_FUNC (ithought_callback_close), NULL);

	vbox = gtk_vbox_new (FALSE, 2);
	gtk_container_add (GTK_CONTAINER (main_window), vbox);

	menubar = gtk_menu_bar_new();
	gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);

	main_file_menu = gtk_menu_new();

	generic = gtk_menu_item_new_with_label ("File");
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (generic), main_file_menu);

	gtk_menu_bar_append (GTK_MENU_BAR (menubar), generic);

	generic = gtk_menu_item_new_with_label ("Quit");
	gtk_signal_connect (GTK_OBJECT (generic), "activate",
	                    GTK_SIGNAL_FUNC (ithought_callback_close), NULL);
	gtk_menu_append (GTK_MENU (main_file_menu), generic);

	main_notebook = gtk_notebook_new();
	gtk_box_pack_start (GTK_BOX (vbox), main_notebook, TRUE, TRUE, 0);
}

static void
ithought_ui_create_entries_tab (void)
{
	gchar *titles[] = {"Entry", "Creation Date", "Last Modified", NULL};
	GtkWidget *vbox, *hbox, *paned, *generic, *scroller;

	paned = gtk_vpaned_new();
	gtk_paned_set_gutter_size (GTK_PANED (paned), 15);
	gtk_paned_set_position (GTK_PANED (paned), 150);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_paned_add1 (GTK_PANED(paned), vbox);

	hbox = gtk_hbox_new (FALSE, 5);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

	generic = gtk_label_new ("Search Term:");
	gtk_box_pack_start (GTK_BOX (hbox), generic, FALSE, FALSE, 2);

	entries_tab_search_term = gtk_entry_new();
	gtk_signal_connect_after (GTK_OBJECT (entries_tab_search_term),
	                          "key_press_event",
							  GTK_SIGNAL_FUNC (
							  ithought_ui_entries_clist_populate), NULL);
	gtk_box_pack_start (GTK_BOX (hbox), entries_tab_search_term,
	                    FALSE, FALSE, 0);

	entries_tab_count_lbl = gtk_label_new ("0 / 0");
	gtk_box_pack_end (GTK_BOX (hbox), entries_tab_count_lbl, FALSE, FALSE, 2);

	scroller = ithought_ui_new_scroller();
	gtk_box_pack_start_defaults (GTK_BOX (vbox), scroller);

	entries_tab_clist = gtk_clist_new_with_titles (3, titles);
	gtk_clist_set_selection_mode (GTK_CLIST (entries_tab_clist),
	                              GTK_SELECTION_SINGLE);
	gtk_clist_set_column_width (GTK_CLIST (entries_tab_clist), 0, 390);
	gtk_clist_set_column_width (GTK_CLIST (entries_tab_clist), 1, 80);
	gtk_clist_set_column_width (GTK_CLIST (entries_tab_clist), 2, 80);
	gtk_signal_connect (GTK_OBJECT (entries_tab_clist), "button_press_event",
	                    GTK_SIGNAL_FUNC (ithought_callback_entries_clist_click),
						NULL);
	gtk_signal_connect (GTK_OBJECT (entries_tab_clist), "click-column",
	                    GTK_SIGNAL_FUNC (ithought_callback_entries_sort), NULL);
	gtk_signal_connect (GTK_OBJECT (entries_tab_clist), "select_row",
	                    GTK_SIGNAL_FUNC (ithought_callback_entries_clist_sel),
						NULL);
	gtk_signal_connect (GTK_OBJECT (entries_tab_clist), "unselect_row",
	                    GTK_SIGNAL_FUNC (ithought_callback_entries_clist_unsel),
						NULL);
	gtk_container_add (GTK_CONTAINER (scroller), entries_tab_clist);

	vbox = gtk_vbox_new (FALSE, 2);
	gtk_paned_add2 (GTK_PANED (paned), vbox);

	hbox = gtk_hbox_new (FALSE, 5);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

	generic = gtk_label_new ("Entry Title:");
	gtk_box_pack_start (GTK_BOX (hbox), generic, FALSE, FALSE, 0);

	entries_tab_entry_title = gtk_entry_new();
	gtk_signal_connect_after (GTK_OBJECT (entries_tab_entry_title),
	                          "key_press_event",
							  GTK_SIGNAL_FUNC (ithought_callback_rename_kpress),
							  NULL);
	gtk_box_pack_start_defaults (GTK_BOX (hbox), entries_tab_entry_title);

	entries_tab_delete_btn = gtk_button_new_with_label ("Delete");
	gtk_signal_connect (GTK_OBJECT (entries_tab_delete_btn), "clicked",
	                    GTK_SIGNAL_FUNC (ithought_callback_delete_btn), NULL);
	gtk_box_pack_end (GTK_BOX (hbox), entries_tab_delete_btn, FALSE, FALSE, 0);

	generic = gtk_button_new_with_label ("New");
	gtk_signal_connect (GTK_OBJECT (generic), "clicked",
	                    GTK_SIGNAL_FUNC (ithought_callback_new_btn), NULL);
	gtk_box_pack_end (GTK_BOX (hbox), generic, FALSE, FALSE, 0);

	scroller = ithought_ui_new_scroller();
	gtk_box_pack_start_defaults (GTK_BOX (vbox), scroller);

	entries_tab_text = gtk_text_new (NULL, NULL);
	gtk_text_set_word_wrap (GTK_TEXT (entries_tab_text), TRUE);
	gtk_container_add (GTK_CONTAINER (scroller), entries_tab_text);

	gtk_notebook_append_page (GTK_NOTEBOOK (main_notebook), paned,
	                          gtk_label_new ("Entries"));
}

static void
ithought_ui_create_actions_tab (void)
{
	GtkWidget		*vbox, *table, *scroller, *generic;
	GSList			*i;
	ModuleAction	*module_action;
	ScriptAction	*script_action;
	gchar			*data[1];
	gint			row;
	
	table = gtk_table_new (2, 3, TRUE);
	
	vbox = gtk_vbox_new (FALSE, 5);

	generic = gtk_label_new ("Action Modules:");
	gtk_box_pack_start (GTK_BOX (vbox), generic, FALSE, FALSE, 0);

	scroller = ithought_ui_new_scroller();
	gtk_box_pack_start (GTK_BOX (vbox), scroller, TRUE, TRUE, 0);

	actions_tab_modules_clist = gtk_clist_new (1);
	gtk_clist_set_selection_mode (GTK_CLIST (actions_tab_modules_clist),
	                              GTK_SELECTION_MULTIPLE);
	gtk_clist_set_column_width (GTK_CLIST (actions_tab_modules_clist), 0, 100);
	gtk_container_add (GTK_CONTAINER (scroller), actions_tab_modules_clist);

	gtk_table_attach_defaults (GTK_TABLE (table), vbox, 0, 1, 0, 1);

	vbox = gtk_vbox_new (FALSE, 5);

	generic = gtk_label_new ("Action Scripts:");
	gtk_box_pack_start (GTK_BOX (vbox), generic, FALSE, FALSE, 0);

	scroller = ithought_ui_new_scroller();
	gtk_box_pack_start_defaults (GTK_BOX (vbox), scroller);

	actions_tab_scripts_clist = gtk_clist_new (1);
	gtk_clist_set_selection_mode (GTK_CLIST (actions_tab_scripts_clist),
	                              GTK_SELECTION_MULTIPLE);
	gtk_clist_set_column_width (GTK_CLIST (actions_tab_scripts_clist), 0, 100);
	gtk_container_add (GTK_CONTAINER (scroller), actions_tab_scripts_clist);

	gtk_table_attach_defaults (GTK_TABLE (table), vbox, 0, 1, 1, 2);

	actions_tab_module_info_lbl = gtk_label_new (NULL);
	gtk_label_set_justify (GTK_LABEL (actions_tab_module_info_lbl),
	                       GTK_JUSTIFY_LEFT);
	gtk_label_set_line_wrap (GTK_LABEL (actions_tab_module_info_lbl), TRUE);
	gtk_table_attach_defaults (GTK_TABLE (table), actions_tab_module_info_lbl,
	                           1, 3, 0, 2);

	for (i = gModuleActionsList; i; i = i->next) {
		module_action = i->data;
		data[0] = module_action->menu_title();

		row = gtk_clist_append (GTK_CLIST (actions_tab_modules_clist), data);
		gtk_clist_set_row_data (GTK_CLIST (actions_tab_modules_clist), row,
		                        module_action);
	}
	gtk_clist_select_all (GTK_CLIST (actions_tab_modules_clist));

	for (i = gScriptActionsList; i; i = i->next) {
		script_action = i->data;
		data[0] = script_action->title;

		row = gtk_clist_append (GTK_CLIST (actions_tab_scripts_clist), data);
		gtk_clist_set_row_data (GTK_CLIST (actions_tab_scripts_clist), row,
		                        script_action);
	}
	gtk_clist_select_all (GTK_CLIST (actions_tab_scripts_clist));

	gtk_signal_connect (GTK_OBJECT (actions_tab_modules_clist), "select_row",
                        GTK_SIGNAL_FUNC (ithought_callback_actions_clist_sel),
						NULL);
	gtk_signal_connect (GTK_OBJECT (actions_tab_modules_clist), "unselect_row",
                        GTK_SIGNAL_FUNC (ithought_callback_actions_clist_unsel),
						NULL);

	gtk_signal_connect (GTK_OBJECT (actions_tab_scripts_clist), "select_row",
	                    GTK_SIGNAL_FUNC (ithought_callback_actions_clist_sel),
						NULL);
	gtk_signal_connect (GTK_OBJECT (actions_tab_scripts_clist), "unselect_row",
	                    GTK_SIGNAL_FUNC (ithought_callback_actions_clist_unsel),
						NULL);

	gtk_notebook_append_page (GTK_NOTEBOOK (main_notebook), table,
	                          gtk_label_new ("Actions"));
}

static void
ithought_ui_create_actions_menu (void)
{
	GtkWidget		*generic, *clist;
	GSList			*i;
	ModuleAction	*module_action;
	ScriptAction	*script_action;
	gchar			*data[1];
	gint			row = 0;

	actions_popup_menu = gtk_menu_new();

	actions_popup_entry_title = gtk_menu_item_new_with_label ("");

	generic = gtk_menu_item_new();
	gtk_menu_append (GTK_MENU (actions_popup_menu), generic);
	gtk_widget_show (generic);

	clist = actions_tab_modules_clist;
	while ((module_action = gtk_clist_get_row_data (GTK_CLIST (clist), row++))){
		gtk_menu_append (GTK_MENU (actions_popup_menu),
		                 module_action->menu_item);
		gtk_widget_show (module_action->menu_item);
	}

	generic = gtk_menu_item_new();
	gtk_menu_append (GTK_MENU (actions_popup_menu), generic);
	gtk_widget_show (generic);

	row = 0;
	clist = actions_tab_scripts_clist;
	while ((script_action = gtk_clist_get_row_data (GTK_CLIST (clist), row++))){
		gtk_menu_append (GTK_MENU (actions_popup_menu),
		                 script_action->menu_item);
		gtk_widget_show (script_action->menu_item);
	}
}

static GtkWidget *
ithought_ui_new_scroller (void)
{
	GtkWidget *scroller;

	scroller = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroller),
	                                GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	return (scroller);
}
