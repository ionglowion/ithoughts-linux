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
#include <sys/stat.h>
#include <sys/types.h>

#include <libxml/tree.h>
#include <libxml/parser.h>

#include "ithought.h"

/* globals */
GSList	*ret_list;

/* static prototypes */
static void ithought_file_create_new();
static void ithought_file_SAX_startElement (void *, const xmlChar *,
                                            const xmlChar **);
static void ithought_file_SAX_endElement (void *, const xmlChar *);
static void ithought_file_SAX_characters (void *, const xmlChar *, int);

/* SAX Handler */
xmlSAXHandler ithoughtSAXHandlerStruct = {
    NULL, /* internalSubset */
    NULL, /* isStandalone */
    NULL, /* hasInternalSubset */
    NULL, /* hasExternalSubset */
    NULL, /* resolveEntity */
    NULL, /* getEntity */
    NULL, /* entityDecl */
    NULL, /* notationDecl */
    NULL, /* attributeDecl */
    NULL, /* elementDecl */
    NULL, /* unparsedEntityDecl */
    NULL, /* setDocumentLocator */
    NULL, /* startDocument */
    NULL, /* endDocument */
    ithought_file_SAX_startElement, /* startElement */
    ithought_file_SAX_endElement, /* endElement */
    NULL, /* reference */
    ithought_file_SAX_characters, /* characters */
    NULL, /* ignorableWhitespace */
    NULL, /* processingInstruction */
    NULL, /* comment */
    NULL, /* xmlParserWarning */
    NULL, /* xmlParserError */
    NULL, /* xmlParserError */
    NULL, /* getParameterEntity */
    NULL, /* cdataBlock; */
};
xmlSAXHandlerPtr ithoughtSAXHandler = &ithoughtSAXHandlerStruct;

/* functions */

void
ithought_file_init (void)
{
	struct stat	st;
	gchar 		*path, *home;
	int			x;

	LIBXML_TEST_VERSION;

	home = g_getenv ("HOME");

	path = g_strdup_printf ("%s/.ithought", home);
	if (stat (path, &st))
		if (mkdir (path, 0755) < 0)
			g_error ("ithought: failed to create: %s", path);
	g_free (path);

	path = g_strdup_printf ("%s/.ithought/module_actions", home);
	if (stat (path, &st))
		if (mkdir (path, 0755) < 0)
			g_error ("ithought: failed to create: %s", path);
	g_free (path);

	path = g_strdup_printf ("%s/.ithought/script_actions", home);
	if (stat (path, &st))
		if (mkdir (path, 0755) < 0)
			g_error ("ithought: failed to create: %s", path);
	g_free (path);

	gXmlFile = g_strdup_printf ("%s/.ithought/ithought_db.xml", home);
	if (stat (gXmlFile, &st))
		ithought_file_create_new (gXmlFile);
}

GSList *
ithought_file_load (gchar *path)
{
	g_slist_free (ret_list);
	ret_list = NULL;
	xmlSAXUserParseFile (ithoughtSAXHandler, NULL, path);
	return (ret_list);
}

void
ithought_file_save (void)
{
	Entry		*my_data;
	GSList		*i;
	xmlDocPtr	doc;
	xmlNodePtr	entry;

	doc = xmlNewDoc ("1.0");
	doc->children = xmlNewDocNode (doc, NULL, "ENTRIES", NULL);

	for (i = gEntriesList; i; i = i->next) {
		my_data = i->data;

		entry = xmlNewChild (doc->children, NULL, "ENTRY", NULL);
		xmlNewTextChild (entry, NULL, "name", my_data->name);
		xmlNewTextChild (entry, NULL, "date", my_data->date);
		xmlNewTextChild (entry, NULL, "mod",  my_data->mod );
		xmlNewTextChild (entry, NULL, "text", my_data->text);
	}
	xmlSaveFile (gXmlFile, doc);
	xmlFreeDoc (doc);
}

/* static functions */

static void
ithought_file_create_new (char *path)
{
	xmlDocPtr	doc;
	xmlNodePtr	entry;

	doc = xmlNewDoc ("1.0");
	doc->children = xmlNewDocNode (doc, NULL, "ENTRIES", NULL);

	entry = xmlNewChild (doc->children, NULL, "ENTRY", NULL);
	xmlNewTextChild (entry, NULL, "name", "Example Entry");
	xmlNewTextChild (entry, NULL, "date", "2000.01.01");
	xmlNewTextChild (entry, NULL, "mod",  "2000.01.01");
	xmlNewTextChild (entry, NULL, "text", "This is an example entry.");

	entry = xmlNewChild (doc->children, NULL, "ENTRY", NULL);
	xmlNewTextChild (entry, NULL, "name", "Another Entry");
	xmlNewTextChild (entry, NULL, "date", "2000.01.01");
	xmlNewTextChild (entry, NULL, "mod",  "2000.01.01");
	xmlNewTextChild (entry, NULL, "text", "This is another entry.");

	xmlSaveFile (path, doc);
	xmlFreeDoc (doc);
}

/* libxml SAX callback related variables and functions */
Entry	*entry;
GString	*buffer;

static void ithought_file_SAX_startElement (void *ctx, const xmlChar *fullname,
                                            const xmlChar **atts)
{
	if (!strcmp (fullname, "ENTRY")) {
		entry = g_malloc (sizeof (Entry));
		return;
	}

	if (!strcmp (fullname, "name") || !strcmp (fullname, "date") ||
	    !strcmp (fullname, "mod" ) || !strcmp (fullname, "text"))
	{
		buffer = g_string_new (NULL);
	}
}

static void ithought_file_SAX_endElement (void *ctx, const xmlChar *fullname)
{
	if (!strcmp (fullname, "name")) {
		entry->name = g_strdup (buffer->str);
		g_string_free (buffer, TRUE);
		return;
	} if (!strcmp (fullname, "date")) {
		entry->date = g_strdup (buffer->str);
		g_string_free (buffer, TRUE);
		return;
	} if (!strcmp (fullname, "mod" )) {
		entry->mod  = g_strdup (buffer->str);
		g_string_free (buffer, TRUE);
		return;
	} if (!strcmp (fullname, "text")) {
		entry->text = g_strdup (buffer->str);
		g_string_free (buffer, TRUE);
		return;
	} if (!strcmp (fullname, "ENTRY")) {
		ret_list = g_slist_append (ret_list, entry);
	}
}

static void ithought_file_SAX_characters (void *ctx, const xmlChar *ch,
                                          int len)
{
	static gchar *tmp_string;

	if (len <= 1 && *ch == '\n')
		return;

	tmp_string = g_strndup (ch, len);
	g_string_append (buffer, tmp_string);
	g_free (tmp_string);
}
