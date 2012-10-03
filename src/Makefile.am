AM_CPPFLAGS = -I$(top_srcdir)/common -I$(top_srcdir)/lib

DEFS = -DLIBDIR=\"$(pkglibdir)\" \
       @DEFS@

bin_PROGRAMS = dbf2txt

dbf2txt_SOURCES = \
   main.c 

dbf2txt_LDADD = \
   ../lib/libdbf2txt.la
        
