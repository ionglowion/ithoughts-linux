bin_PROGRAMS = ithought

ithought_SOURCES =\
	action.c \
	action.h \
	callback.c \
	callback.h \
	entry.c \
	entry.h \
	file.c \
	file.h \
	ithought.c \
	ithought.h \
	ui.c \
	ui.h

ithought_LDADD = $(GTK_LIBS) $(XML2_LIBS)

INCLUDES = \
$(GTK_INCLUDEDIR) $(XML2_INCLUDEDIR) -I$(includedir) \
-DDATADIR=\"$(prefix)/share\"
