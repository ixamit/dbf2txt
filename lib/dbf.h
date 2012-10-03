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

#ifndef _DBF_H
#define _DBF_H
#ifndef ui16
#define ui16 u_int16_t
#endif
#ifndef ui32
#define ui32 u_int32_t
#endif

typedef char DBF_RECORD;
typedef struct sub_header
{
  /*
  Field name with a maximum of 10 characters. 
  If less than 10, it is padded with null characters (0x00).
  */
  char FieldName[11];

  /*
  Field type:
  C Character
  Y Currency
  N Numeric
  F Float
  D Date
  T DateTime
  B Double
  I Integer
  L Logical
  M Memo
  G General
  C Character (binary)
  M Memo (binary)
  P Picture
  + Autoincrement (dBase Level 7)
  O Double (dBase Level 7)
  @ Timestamp (dBase Level 7)
  */
  unsigned char FieldType;

  /*
   Warning: 
   Clipper doesnt seems to assign DisplacementOfFieldInRecord
  */
  char DisplacementOfFieldInRecord[4];
  
  /*
   Warning!
   Clipper uses for field type 'C' LengthOfField and NumberOfDecimalPlaces together as
   type ui16 as field length for fields bigger than 255 chars.
   So, dont read LengthOfField from here but use function `LengthOfField (DBF_SUBHEADER *sub_header)` instead
  */
  unsigned char LengthOfField;          // please, read warning above
  unsigned char NumberOfDecimalPlaces;  // please, read warning above
  
  /*
  Field flags:
  0x01   System Column (not visible to user)
  0x02   Column can store null values
  0x04   Binary column (for CHAR and MEMO only)
  0x06   (0x02+0x04) When a field is NULL and binary (Integer, Currency, and Character/Memo fields)
  0x0C   Column is autoincrementing
  */
  unsigned char FieldFlags;

  char ValueOfAutoIncrementNextValue[4];
  unsigned char ValueOfAutoIncrementStepValue;

  ui16 RealLengthOfField;
  char Reserved[6];
  
}DBF_SUBHEADER;

typedef struct header
{
  /*
  DBF File type:
  0x02   FoxBASE
  0x03   FoxBASE+/Dbase III plus, no memo
  0x30   Visual FoxPro
  0x31   Visual FoxPro, autoincrement enabled
  0x32   Visual FoxPro with field type Varchar or Varbinary
  0x43   dBASE IV SQL table files, no memo
  0x63   dBASE IV SQL system files, no memo
  0x83   FoxBASE+/dBASE III PLUS, with memo
  0x8B   dBASE IV with memo
  0xCB   dBASE IV SQL table files, with memo
  0xF5   FoxPro 2.x (or earlier) with memo
  0xE5   HiPer-Six format with SMT memo file
  0xFB   FoxBASE
  */
  unsigned char FileType;

  /*
  Last update:
  YYMMDD
  */
  char LastUpdate[3];

  ui32 NumberOfRecords;
  ui16 PositionOfFirstDataRecord;
  
  ui16 LengthOfOneDataRecord;     // Including delete 1 byte flag
  char Reserved1[16];
  
  /*
  Table flags:
  0x01   file has a structural .cdx
  0x02   file has a Memo field
  0x04   file is a database (.dbc)
  This byte can contain the sum of any of the above values. 
  For example, the value 0x03 indicates the table has a structural .cdx and a Memo field.
  */
  unsigned char TableFlags;

  unsigned char CodePageMark;

  /*
  Reserved, contains 0x00
  */
  char Reserved2[2];
  
}DBF_HEADER;

ui16 LengthOfField (DBF_SUBHEADER *sub_header);
ui16 NumberOfDecimalPlaces (DBF_SUBHEADER *sub_header);

int  _dbf_init (FILE *fp, DBF_HEADER **header, DBF_SUBHEADER ***sub_header, int *NumberOfFields, DBF_RECORD **record);
int  _dbf_load_record (FILE *fp, DBF_HEADER *header, char *record, ui32 record_number, size_t size);
int  _dbf_isdelete (DBF_RECORD *record);
int  _dbf_isnumeric (DBF_SUBHEADER *sub_header);
int  _dbf_ischar (DBF_SUBHEADER *sub_header);
void _dbf_end (DBF_HEADER *header, DBF_SUBHEADER **sub_header, int NumberOfFields, DBF_RECORD *record);

#endif // _DBF_H