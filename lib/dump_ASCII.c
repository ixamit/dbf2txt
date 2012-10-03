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
// Module ASCII vers 0.2
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

void dbf_dump_ASCII_HEADandTAIL (DBF *dbf,int n)
{
  FILE *fp=stdout;
  int i,j,l;
  
  
  for (j=0;j<n;j++)
  {
    for (i=0;i<dbf->nselect;i++)
    {
      l=dbf->sub_header[dbf->select[i]]->RealLengthOfField;
      if (strlen(dbf->sub_header[dbf->select[i]]->FieldName)>l)
        l=strlen(dbf->sub_header[dbf->select[i]]->FieldName);
      l+=2;
      
      if (j!=1)
      {
        putc ('+',fp);putnc ('-',l,fp);
      }
      else
      {
        putc ('|',fp); putc(' ',fp); fputs(dbf->sub_header[dbf->select[i]]->FieldName,fp); putc(' ',fp);
        l-=(strlen(dbf->sub_header[dbf->select[i]]->FieldName)+2);
        if (l>0)
          putnc (' ',l,fp);
      }
    }
    if (j==1)
      putc ('|',fp);
    else
      putc ('+',fp);
    putc ('\n',fp);
  }

}
void dbf_dump_ASCII_HEAD (DBF *dbf)
{
  dbf_dump_ASCII_HEADandTAIL (dbf,3);
}
void dbf_dump_ASCII_TAIL (DBF *dbf)
{
  dbf_dump_ASCII_HEADandTAIL (dbf,1);
}
void dbf_dump_ASCII_BODY (DBF *dbf,int RecNumber)
{
  int k,i,j;
  ui32 position;
  ui16 len;
  char format[16];
  
  if (dbf==NULL)
    return;
  
  for (k=0;k<dbf->nselect;k++)
  {
    i=dbf->select[k];
    memcpy (&position,dbf->sub_header[i]->DisplacementOfFieldInRecord,sizeof(position));
    
    len=LengthOfField(dbf->sub_header[i]);
    memcpy (dbf->tmp_sub_record,dbf->record+position,len); dbf->tmp_sub_record[len]=0; 
    j=0;
    
    if (_dbf_isnumeric(dbf->sub_header[i]))
    {
      // NUMERIC
      sprintf (format, "| @%ds ",dbf->sub_header[i]->RealLengthOfField);
      j=len-dbf->sub_header[i]->RealLengthOfField;
      if (j<0)
        j=0;
    }
    else
    { // ALPHA NOT NUMERIC
      sprintf (format, "| @-%ds ",dbf->sub_header[i]->RealLengthOfField);
      dbf->tmp_sub_record[dbf->sub_header[i]->RealLengthOfField]=0;
    }
    *strchr(format,'@')='%';
    printf (format,dbf->tmp_sub_record+j);
  }
  printf ("|\n");
  
}
