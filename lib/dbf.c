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
#include "dbf.h"
#include "mem.h"

ui16 LengthOfField (DBF_SUBHEADER *sub_header)
{
  return (ui16) ((sub_header)?(sub_header->FieldType=='C')?sub_header->NumberOfDecimalPlaces*256+sub_header->LengthOfField:sub_header->LengthOfField:0);
}
ui16 NumberOfDecimalPlaces (DBF_SUBHEADER *sub_header)
{
  return (ui16) ((sub_header)?(sub_header->FieldType=='N'||sub_header->FieldType=='F'||sub_header->FieldType=='B'||sub_header->FieldType=='I'||sub_header->FieldType=='O')?sub_header->NumberOfDecimalPlaces:0:0);
}

int _dbf_load_subheader (FILE *fp, DBF_SUBHEADER **sub_header)
{
  if ((*sub_header=(DBF_SUBHEADER *) xmalloc (sizeof(DBF_SUBHEADER)))==NULL)
  {
    perror ("xmalloc");
    return -1;
  }

  if ((fread(*sub_header,sizeof(DBF_SUBHEADER),1,fp))!=1)
  {
    perror ("fread");
    return -1;
  }

 /*
  fread(&(*sub_header)->FieldName,sizeof((*sub_header)->FieldName),1,fp);
  fread(&(*sub_header)->FieldType,sizeof((*sub_header)->FieldType),1,fp);
  fread(&(*sub_header)->DisplacementOfFieldInRecord,sizeof((*sub_header)->DisplacementOfFieldInRecord),1,fp);
  fread(&(*sub_header)->LengthOfField,sizeof((*sub_header)->LengthOfField),1,fp);
  fread(&(*sub_header)->NumberOfDecimalPlaces,sizeof((*sub_header)->NumberOfDecimalPlaces),1,fp);
  fread(&(*sub_header)->FieldFlags,sizeof((*sub_header)->FieldFlags),1,fp);
  fread(&(*sub_header)->ValueOfAutoIncrementNextValue,sizeof((*sub_header)->ValueOfAutoIncrementNextValue),1,fp);
  fread(&(*sub_header)->ValueOfAutoIncrementStepValue,sizeof((*sub_header)->ValueOfAutoIncrementStepValue),1,fp);
  fread(&(*sub_header)->RealLengthOfField,sizeof((*sub_header)->RealLengthOfField),1,fp);
  fread(&(*sub_header)->Reserved,sizeof((*sub_header)->Reserved),1,fp);
*/
  return 0;
}

void _dbf_free_header (DBF_HEADER *header)
{
  free (header);
}
void _dbf_free_subheader (DBF_SUBHEADER **sub_header, int NumberOfFields)
{
  for (;NumberOfFields--;)
    free (sub_header[NumberOfFields]);
  free (sub_header);
}
void _dbf_free_record (DBF_RECORD *record)
{
  free (record);
}

void _dbf_end (DBF_HEADER *header, DBF_SUBHEADER **sub_header, int NumberOfFields, DBF_RECORD *record)
{
  _dbf_free_header (header);
  _dbf_free_subheader (sub_header,NumberOfFields);
  _dbf_free_record (record);
}
int _dbf_init (FILE *fp, DBF_HEADER **header, DBF_SUBHEADER ***sub_header, int *NumberOfFields, DBF_RECORD **record)
{
  int i;
  ui32 position=1;
  
  *NumberOfFields=0;

   /*
   * Alloc header
   */
  if ((*header=(DBF_HEADER *) xmalloc (sizeof(DBF_HEADER)))==NULL)
  {
    perror ("xmalloc");
    return -1;
  }
  if ((fread(*header,sizeof(DBF_HEADER),1,fp))!=1)
  {
    perror ("fread ");
    return -1;
  }
  if (!strchr("\x02\x03\x30\x31\x32\x43\x63\x83\x8B\xCB\xF5\xE5\xFB",(*header)->FileType))
  {
    fprintf (stderr,"Error: invalid File Type (`0x%02X`)\n",(*header)->FileType);
    _dbf_free_header (*header);
    return -1;
  }
  /*
   * Alloc one record
   */
  if ((*record=(DBF_RECORD *) xmalloc ((*header)->LengthOfOneDataRecord))==NULL)
  {
    perror ("xmalloc DBF_RECORD");
    return -1;
  }

  /* 
   * Long Way to set the NumberOfFields 
   * [-2  is header terminator (0x0D + 0x00)]
   */
  *NumberOfFields=(((*header)->PositionOfFirstDataRecord)-sizeof(DBF_HEADER)-2)/sizeof(DBF_SUBHEADER);
  
  /*
   * Alloc subrecord
   */
  if ((*sub_header=(DBF_SUBHEADER **) xmalloc (*NumberOfFields * sizeof(DBF_SUBHEADER *)))==NULL)
  {
    perror ("xmalloc DBF_SUBHEADER");
    return -1;
  }

  /*
  * Setting DisplacementOfFieldInRecord
  * with 'REAL' field position
  */
  for (i=0; i < *NumberOfFields ;i++)
  {
    if ((_dbf_load_subheader (fp,*sub_header+i))!=0)
      return -1;
    
    memcpy((*sub_header)[i]->DisplacementOfFieldInRecord,&position,sizeof(ui32));
    position+=LengthOfField((*sub_header)[i]);
    
    (*sub_header)[i]->RealLengthOfField=strlen((*sub_header)[i]->FieldName);
  }

  return 0;
}

int _dbf_load_record (FILE *fp, DBF_HEADER *header, char *record, ui32 record_number, size_t size)
{
  long int offset;
  int err=-1;
  
  if (record_number>0 && record_number<=header->NumberOfRecords)
  {
    offset=header->PositionOfFirstDataRecord+(header->LengthOfOneDataRecord * (record_number-1));
    if ((fseek (fp,offset,SEEK_SET))==0)
    {
      if ((fread(record,size,1,fp))==1)
        err=0;
      else
        perror ("fread");
    }
    else
      perror ("fseek");
  }
  else  
    fprintf (stderr,"Error: `%d`invalid record number\n",record_number);
  
  return err;
}

int _dbf_isdelete (DBF_RECORD *record)
{
  return (*record!=' ')?1:0;
}

int _dbf_isnumeric (DBF_SUBHEADER *sub_header)
{
  return 
    (
      sub_header->FieldType=='N'||
      sub_header->FieldType=='F'||
      sub_header->FieldType=='B'||
      sub_header->FieldType=='I'||
      sub_header->FieldType=='O' 
     ) ?1:0;
}
int _dbf_ischar (DBF_SUBHEADER *sub_header)
{
  return 
    (
      sub_header->FieldType=='C'
     ) ?1:0;
}

