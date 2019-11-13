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

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ithought.h"

#define SHARED_LIB_EXT ".so"

/* static prototypes */
static void	ithought_action_read_module_dir	(gchar *);
static void	ithought_action_read_script_dir	(gchar *);

/* functions */

void
ithought_action_init (void)
{
	gchar *path;

	path = g_strdup_printf ("%s/ithought/module_actions", DATADIR);
	ithought_action_read_module_dir (path);
	/* g_free (path); this was giving a "chuck is already free" warning */

	path = g_strdup_printf ("%s/.ithought/module_actions", g_getenv ("HOME"));
	ithought_action_read_module_dir (path);
	/* g_free (path); this was giving a "chunk is already free" warning */

	path = g_strdup_printf ("%s/ithought/script_actions", DATADIR);
	ithought_action_read_script_dir (path);
	g_free (path);

	path = g_strdup_printf ("%s/.ithought/script_actions", g_getenv ("HOME"));
	ithought_action_read_script_dir (path);
	g_free (path);
}

void
ithought_action_do_script (GtkWidget *widget, ScriptAction *action)
{
	pid_t	child;
	gint	fds[2];
	gchar	buf[100];
	gint	status;
	ssize_t	bytes_read;

	pipe (fds);

	switch ((child = fork())) {
		case -1:
			g_error ("ithought: fork() failed");
		case 0:
			close (fds[0]);
			fflush (stdout);
			dup2 (fds[1], STDOUT_FILENO);
			close (fds[1]);

			execl (action->file_path, action->file_path, "action", gActionText,
			       NULL);
			break;
		default:
			close (fds[1]);
			while ((bytes_read = read (fds[0], &buf, sizeof (buf) - 1))) {
				buf[bytes_read] = '\0';
				printf ("%s", buf);
			}
			waitpid (child, &status, 0);
			if (status)
				g_warning ("ithought: waitpid() received status: %d", status);
			break;
	}
}

/* static functions */

static void
ithought_action_read_module_dir (gchar *path)
{
	DIR				*action_dir;
	struct dirent	*dp;
	struct stat		sb;
	gchar			*action_path, *ext;
	gpointer		sym;

	if (!g_module_supported())
		g_error ("ithought: %s", g_module_error());
	
	if (!(action_dir = opendir (path))) {
		g_warning ("ithought: opendir() failed on %s", path);
		return;
	}

	while ((dp = readdir (action_dir))) {
		ModuleAction *tmp;

		action_path = g_strdup_printf ("%s/%s", path, dp->d_name);
		if (!stat (action_path, &sb) && S_ISREG (sb.st_mode) &&
		    (ext = strchr (dp->d_name, '.')) && !strcmp (ext, SHARED_LIB_EXT)) {

			tmp = g_malloc (sizeof (ModuleAction));
			if (!(tmp->module = g_module_open (action_path, 0)))
				g_error ("ithought: %s", g_module_error());

			if (!g_module_symbol (tmp->module, "module_action", &sym))
				g_error ("ithought: %s", g_module_error());
			tmp->do_action = (void (*)(char *))sym;

			if (!g_module_symbol (tmp->module, "get_info", &sym))
				g_error ("ithought: %s", g_module_error());
			tmp->get_info = (char *(*)(void))sym;
			
			if (!g_module_symbol (tmp->module, "menu_title", &sym))
				g_error ("ithought: %s", g_module_error());
			tmp->menu_title = (char *(*)(void))sym;

			tmp->menu_item = gtk_menu_item_new_with_label (tmp->menu_title());
			gtk_signal_connect (GTK_OBJECT (tmp->menu_item), "activate",
			                    GTK_SIGNAL_FUNC (tmp->do_action), &gActionText);

			gModuleActionsList = g_slist_append (gModuleActionsList, tmp);
		}
		g_free (action_path);
	}
	g_free (path);
	closedir (action_dir);
}

static void
ithought_action_read_script_dir (gchar *path)
{
	DIR				*action_dir;
	struct dirent	*dp;
	struct stat		sb;
	FILE			*fp;
	gchar			*action_path, *command;
	gchar			buf[100];
	GString			*string;

	if (!(action_dir = opendir (path))) {
		g_warning ("ithought: opendir() failed on %s", path);
		return;
	}

	while ((dp = readdir (action_dir))) {
		ScriptAction *tmp;

		action_path = g_strdup_printf ("%s/%s", path, dp->d_name);
		if (!stat (action_path, &sb) && S_ISREG (sb.st_mode)) {
			tmp = g_malloc (sizeof (ScriptAction));
			tmp->file_path = g_strdup (action_path);

			command = g_strdup_printf ("%s title", action_path);
			if (!(fp = popen (command, "r")))
				g_error ("ithought: popen() failed on: %s", command);

			string = g_string_new (NULL);
			if (fgets (buf, sizeof (buf) - 1, fp))
				string = g_string_append (string, g_strchomp (buf));
			tmp->title = g_strdup (string->str);

			g_string_free (string, TRUE);
			g_free (command);
			pclose (fp);

			command = g_strdup_printf ("%s info", action_path);
			if (!(fp = popen (command, "r")))
				g_error ("ithought: popen() failed on: %s", command);

			string = g_string_new (NULL);
			while (fgets (buf, sizeof (buf) - 1, fp)) {
				g_string_sprintfa (string, "%s\n", buf);
			}
			tmp->info = g_strdup (string->str);

			g_string_free (string, TRUE);
			g_free (command);
			pclose (fp);
			
			tmp->menu_item = gtk_menu_item_new_with_label (tmp->title);
			gtk_signal_connect (GTK_OBJECT (tmp->menu_item), "activate",
			                    GTK_SIGNAL_FUNC (ithought_action_do_script),
								tmp);
			
			gScriptActionsList = g_slist_append (gScriptActionsList, tmp);
		}
		g_free (action_path);
	}
	closedir (action_dir);
}
