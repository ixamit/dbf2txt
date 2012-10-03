noinst_LTLIBRARIES = libcommon.la

DEFS = -DLIBDIR=\"$(pkglibdir)\" \
       @DEFS@

libcommon_la_SOURCES = \
   mem.h mem.c \
   util.h util.c \
   in.h in.c \
   opt.h opt.c \
   explode.h explode.c
 
CLEANFILES = \
   libcommon.a \
   libcommon.la    \
   *~
