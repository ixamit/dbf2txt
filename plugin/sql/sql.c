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
// Module SQL ver 0.1
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
	foo->name="SQL";
	foo->desc="Structured Query Language (SQL)";
	foo->vers="0.1";

	return 0;
}

char * SQLType (DBF_SUBHEADER *sub_header)
{
  static char SQLType[64];
  int l=LengthOfField(sub_header);
  int d=NumberOfDecimalPlaces(sub_header);
  switch (sub_header->FieldType)
  {
    case 'N':
      if (d)
        sprintf (SQLType,"decimal(%d,%d)",l,d);
      else if (l<5)
        sprintf (SQLType,"smallint(%d)",l);
      else if (l<3)
        sprintf (SQLType,"tinyint(%d)",l);
      else
        sprintf (SQLType,"int(%d)",l);
      break;
    case 'C':
      if (l<=255)
        sprintf (SQLType,"varchar(%d) COLLATE utf8_unicode_ci",l);
      else
        sprintf (SQLType,"text(%d)",l);
      break;
    case 'D':
      sprintf (SQLType,"date");
      break;
  }
  return SQLType;
}
void purify (char *s)
{
  char *p;
  while ((p=strchr(s,'"')))
  {
    *p='\'';
  }
}

void SQL_HEAD (DBF *dbf)
{
  int i,k;
  char *TABLE=dbf->database;
  char *ENGINE="MyISAM";
  char *CHARSET="utf8";
  char *COLLATE="COLLATE=utf8_unicode_ci";
    
  printf ("-- SQL dump created by %s version %s\n",PACKAGE,PACKAGE_VERSION);
  printf ("-- please report bugs at %s\n",PACKAGE_BUGREPORT);
  printf ("--\n");
  printf ("-- ----------------------------------------\n");
  printf ("\n");
    
  printf ("--\n");
  printf ("-- Table structure for table `%s`\n",TABLE);
  printf ("--\n\n");
  printf ("DROP TABLE IF EXISTS `%s`;\n",TABLE);
  printf ("CREATE TABLE `%s` (\n",TABLE);
  // Fields loop
  for (k=0;k<dbf->nselect;k++)
  {
    i=dbf->select[k];
    printf ("  `%s` %s",dbf->sub_header[i]->FieldName,SQLType(dbf->sub_header[i]));
    if (k<dbf->nselect-1)
      printf (",");
    printf ("\n");
  }
  // Key Loop
  printf ("\n");
  printf ("  /* Modify and or/Add KEYS */\n");
  printf ("  # PRIMARY KEY (`%s`)\n",dbf->sub_header[0]->FieldName);
    
  printf (") ENGINE=%s DEFAULT CHARSET=%s %s;\n\n",ENGINE,CHARSET,COLLATE);
    
  printf ("--\n");
  printf ("-- Dumping data from table `%s`\n",TABLE);
  printf ("--\n\n");
  printf ("LOCK TABLES `%s` WRITE;\n",TABLE);
  printf ("INSERT INTO `%s` VALUES ",TABLE);
  #if DEBUG    
    printf ("\n");
  #endif    
  
  //
}

void SQL_TAIL (DBF *dbf)
{
  printf ("UNLOCK TABLES;\n\n");
}

void SQL_BODY (DBF *dbf,ui32 RecNumber)
{
  int k,i;
  ui32 position;
  ui16 len;

  printf ("("); // DEBUG \n

  for (k=0;k<dbf->nselect;k++)
  {
    i=dbf->select[k];
    memcpy (&position,dbf->sub_header[i]->DisplacementOfFieldInRecord,sizeof(position));
    
    len=LengthOfField(dbf->sub_header[i]);
    
    memcpy (dbf->tmp_sub_record,dbf->record+position,len); dbf->tmp_sub_record[len]=0; 
    alltrim (dbf->tmp_sub_record);
    

    if (_dbf_isnumeric(dbf->sub_header[i]))
      printf ("%s",(*dbf->tmp_sub_record)?dbf->tmp_sub_record:"0");
    else
    {
      purify (dbf->tmp_sub_record);
      printf ("\"%s\"",dbf->tmp_sub_record); // FIXME Security
    }
    if (k<dbf->nselect-1)
      printf (",");
      
  } 
  printf (")");
  if (RecNumber<dbf->NumberOfRecords)
  {
    printf (","); 
    #if DEBUG    
      printf ("\n"); 
    #endif
  }
  else
    printf (";\n");
}

