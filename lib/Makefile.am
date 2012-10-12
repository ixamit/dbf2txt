AM_CPPFLAGS = -I$(top_srcdir)/common

DEFS = -DLIBDIR=\"$(pkglibdir)\" \
       @DEFS@

pkglib_LTLIBRARIES = libdbf2txt.la

libdbf2txt_la_SOURCES = \
   dbf.h dbf.c \
   select.h select.c \
   conditions.h conditions.c \
   order.h order.c \
   dbf2txt.h dbf2txt.c \
   plugin.h plugin.c \
   dump_ASCII.h dump_ASCII.c 

libdbf2txt_la_LIBADD = \
   ../common/libcommon.la -ldl

CLEANFILES = \
   libdbf2txt.a \
   libdbf2txt.la  \
    *~

