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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "util.h"
#include "mem.h"
#include "dbf.h"
#include "dbf2txt.h"

void free_order (int *order)
{
  free (order);
}
int make_order (char *argv, DBF *dbf) 
{
  int i,j,k,len;
  static char field[256];

  /** alloc array */
  if ((dbf->order=xmalloc (dbf->NumberOfFields*sizeof(int)))==NULL)
    return -1;

  dbf->norder=j=0;
  while (argv[j])
  {
    for (i=j;argv[j] && isspace(argv[j]);j++);
    for (i=j;argv[j] && !isspace(argv[j]);j++);
    
    len=j-i;
    if (len>=sizeof(field))
      len=sizeof(field)-1;
    memcpy (field, argv+i, len);
    field[len]=0;
    
    for (k=0;k<dbf->NumberOfFields;k++)
    {
      if (!xstrcmpi(dbf->sub_header[k]->FieldName,field))
      {
        dbf->order[dbf->norder++]=k;
        break;
      }
    }
    if (k==dbf->NumberOfFields)
    {
      printf ("Invalid `%s` in order\n",field);
      free_order (dbf->order);
      dbf->norder=0;
      return -1;
    }
    
    for (i=j;argv[j] && isspace(argv[j]);j++);
  }
  
  return 0;
}
