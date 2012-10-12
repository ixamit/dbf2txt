/**
 *  Copyright (C) 2012, Max Cavallo <ixamit_at_gmail_dot_com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
*/

///////////////////////////////////////////////////////////////////////////////
//
// Module XML ver 0.1
// 
//
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "mem.h"
#include "util.h"
#include "opt.h"
#include "dbf.h"
#include "dbf2txt.h"
#include "select.h"
#include "order.h"	

#include "plugin.h"
#define DEBUG 1

int plugin_init (PLUGIN *foo)
{
	foo->name="XML";
	foo->desc="Extensible Markup Language (XML)";
	foo->vers="0.1";

	return 0;
}

char * XMLType (DBF_SUBHEADER *sub_header)
{
  static char XMLType[64];
  int l=LengthOfField(sub_header);
  int d=NumberOfDecimalPlaces(sub_header);
  switch (sub_header->FieldType)
  {
    case 'N':
      if (d)
        sprintf (XMLType,"decimal(%d,%d)",l,d);
      else if (l<5)
        sprintf (XMLType,"smallint(%d)",l);
      else if (l<3)
        sprintf (XMLType,"tinyint(%d)",l);
      else
        sprintf (XMLType,"int(%d)",l);
      break;
    case 'C':
      if (l<=255)
        sprintf (XMLType,"varchar(%d)",l);
      else
        sprintf (XMLType,"text(%d)",l);
      break;
    case 'D':
      sprintf (XMLType,"date");
      break;
  }
  return XMLType;
}

void XML_purify (char *s)
{
  char *p;
  while ((p=strchr(s,'"')))
  {
    *p='\'';
  }
}
void XML_HEAD (DBF *dbf)
{
  int i,k;
  char *TABLE=dbf->database;
  //char *ENGINE="MyISAM";
  //char *CHARSET="utf8";
  //char *COLLATE="COLLATE=utf8_unicode_ci";
  
  printf ("<?xml version=\"1.0\"?>\n");
  printf ("<%s xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n",PACKAGE);
  printf ("  <database name=\"%s\">\n","dummy");
  printf ("    <table_structure name=\"%s\">\n",TABLE);
    
  // Fields loop
  for (k=0;k<dbf->nselect;k++)
  {
    i=dbf->select[k];
    printf ("      <field Field=\"%s\" Type=\"%s\" />\n",dbf->sub_header[i]->FieldName,XMLType(dbf->sub_header[i]));
  }
  printf ("    </table_structure>\n");
  printf ("    <table_data name=\"%s\">\n",TABLE);
  //
}

void XML_BODY (DBF *dbf,ui32 RecNumber)
{
  int i,k;
  char *TABLE=dbf->database;
  //char *ENGINE="MyISAM";
  //char *CHARSET="utf8";
  //char *COLLATE="COLLATE=utf8_unicode_ci";
  
  printf ("<?xml version=\"1.0\"?>\n");
  printf ("<%s xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n",PACKAGE);
  printf ("  <database name=\"%s\">\n","dummy");
  printf ("    <table_structure name=\"%s\">\n",TABLE);
    
  // Fields loop
  for (k=0;k<dbf->nselect;k++)
  {
    i=dbf->select[k];
    printf ("      <field Field=\"%s\" Type=\"%s\" />\n",dbf->sub_header[i]->FieldName,XMLType(dbf->sub_header[i]));
  }
  printf ("    </table_structure>\n");
  printf ("    <table_data name=\"%s\">\n",TABLE);
  //
}

void XML_TAIL (DBF *dbf)
{
  printf ("    </table_data>\n");  
  printf ("  </database>\n");
  printf ("</%s>\n",PACKAGE);
}


