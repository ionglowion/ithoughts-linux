AC_DEFUN([GTK_CHECK],
[
	AC_SUBST(GTK_LIBS)
	AC_SUBST(GTK_INCLUDEDIR)
	AC_PATH_PROG(GTK_CONFIG,gtk-config,no)

	if test "$GTK_CONFIG" = "no"; then
		no_gtk_config="yes"
		AC_MSG_ERROR([* GTK not found on this system, unable to build *])
	else
		AC_MSG_CHECKING(if $GTK_CONFIG works)
		if $GTK_CONFIG --libs >/dev/null 2>&1; then
			AC_MSG_RESULT(yes)
			GTK_LIBS="`$GTK_CONFIG --libs`"
			GTK_INCLUDEDIR="`$GTK_CONFIG --cflags`"
		else
			AC_MSG_RESULT(no)
			no_gtk_config="yes"
			AC_MSG_ERROR([* GTK not found on this system, unable to build *])
		fi
	fi
])

AC_DEFUN([XML2_CHECK],
[
	AC_SUBST(XML2_LIBS)
	AC_SUBST(XML2_INCLUDEDIR)
	AC_PATH_PROG(XML2_CONFIG,xml2-config,no)

	if test "$XML2_CONFIG" = "no"; then
		no_xml2_config="yes"
		AC_MSG_ERROR([* libxml2 not found on this system, unable to build *

Please follow the instructions at 'http://www.xmlsoft.org/#Downloads' to
download and install libxml2, then try compiling ithought again.])
	else
		AC_MSG_CHECKING(if $XML2_CONFIG works)
		if $XML2_CONFIG --libs >/dev/null 2>&1; then
			AC_MSG_RESULT(yes)
			XML2_LIBS="`$XML2_CONFIG --libs`"
			XML2_INCLUDEDIR="`$XML2_CONFIG --cflags`"
		else
			AC_MSG_RESULT(no)
			no_gtk_config="yes"
			AC_MSG_ERROR([* libxml2 not found on this system, unable to build *

Please follow the instructions at 'http://www.xmlsoft.org/#Downloads' to
download and install libxml2, then try compiling ithought again.])
		fi
	fi
])
