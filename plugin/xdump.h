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

#define DBNAME                         (dbf->database)
#define DBNRECORDS                     (dbf->NumberOfRecords)
#define DBFIELD_HEADER(i)              (dbf->sub_header[(i)])
#define DBFIELD_NAME(i)                (DBFIELD_HEADER(i)->FieldName)
#define DBFIELD_TYPE(i)                (DBFIELD_HEADER(i)->FieldType)
#define DBFIELD_ISNUMERIC(i)           (_dbf_isnumeric(DBFIELD_HEADER(i)))
#define DBFIELD_LEN(i)                 (LengthOfField(DBFIELD_HEADER((i))))
#define DBFIELD_DEC(i)                 (NumberOfDecimalPlaces(DBFIELD_HEADER((i))))
#define DBFIELD_LOAD_CONTENTS(i) \
	ui32 position; \
	ui16 len; \
	memcpy (&position,dbf->sub_header[(i)]->DisplacementOfFieldInRecord,sizeof(position)); \
	len=LengthOfField(dbf->sub_header[(i)]); \
	memcpy (dbf->tmp_sub_record,dbf->record+position,len); \
	dbf->tmp_sub_record[len]=0; \
	alltrim (dbf->tmp_sub_record); 
#define DBFIELD_CONTENTS               (dbf->tmp_sub_record)
//#define SELECT                         (dbf->select[0])
#define NSELECT                        (dbf->nselect)
unsigned int INDEXOF;
#define FOREACH_SELECT(i) \
	for (INDEXOF=0;INDEXOF<NSELECT && (i=dbf->select[INDEXOF])==i;INDEXOF++)

