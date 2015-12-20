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
#include "conditions.h"

typedef struct dbf
{
  FILE *fp_in;
  char *database;

  DBF_HEADER *header;                    // ::
  DBF_SUBHEADER **sub_header;            // ::
  int NumberOfFields;                    // sub_header counter
  
  DBF_RECORD *record;                    // The Record
  char *tmp_sub_record;                  // BIGgest TMP Record's Field
  ui32 NumberOfRecords;                  // Filtered NumberOfRecords
  
  CONDITION *condition;                  // list  of --WHERE
  int *select;                           // array of --SELECT (sub_header's index)
  int nselect;                           // 
  int *order;                            // array of --ORDER  (sub_header's index)
  int norder;                            //
  
  void (*dbf_dump_HEAD) (struct dbf *);  //  :::
  void (*dbf_dump_BODY) (struct dbf *, ui32);  // Parametric dump zone
  void (*dbf_dump_TAIL) (struct dbf *);  //  :::

}DBF;
int dbf_init (DBF **dbf, char *filename);
void dbf_end (DBF *dbf);
void dbf_show_infofields (DBF *dbf);
int dbf_show_records (DBF *dbf);
