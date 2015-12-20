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

typedef struct _field_type
{
  unsigned char type;
  char long_des [32];
}field_type;
field_type FIELD_TYPE[]=
{
  {'C',"Character"}, 
  {'Y',"Currency "},
  {'N',"Numeric  "},
  {'F',"Float    "},
  {'D',"Date     "},
  {'T',"DateTime "},
  {'B',"Double   "},
  {'I',"Integer  "},
  {'L',"Logical  "},
  {'M',"Memo     "},
  {'G',"General  "},
  {'P',"Picture  "},
  {'+',"Autoinc. "},
  {'O',"Double   "},
  {'@',"Timestamp"},
};
typedef struct _db_type
{
  unsigned char type;
  char long_des [36];
}db_type;
db_type DB_TYPE[]={
  {0x02,"FoxBASE                           "},
  {0x03,"FoxBASE+/dBASE III plus, no memo  "},
  {0x30,"Visual FoxPro                     "},
  {0x31,"Visual FoxPro with autoincrement  "},
  {0x32,"Visual FoxPro Varchar or Varbinary"},
  {0x43,"dBASE IV SQL table files, no memo "},
  {0x63,"dBASE IV SQL system files, no memo"},
  {0x83,"FoxBASE+/dBASE III PLUS, with memo"},
  {0x8B,"dBASE IV with memo                "},
  {0xCB,"dBASE IV SQL table files with memo"},
  {0xF5,"FoxPro 2.x (or earlier) with memo "},
  {0xE5,"HiPer-Six format with SMT memo    "},  
  {0xFB,"FoxBASE                           "},    
};


ui32 char2ui32 (char *p)
{
  ui32 v;
  memcpy(&v,p,sizeof(v));
  return v;
}

ui16 real_len(char *s, char type)
{
  ui16 len=0;
  
  if (s)
  {
    len=strlen(s);
    if (type=='N'||type=='F'||type=='B'||type=='I'||type=='O')
      for (;*s && *s==' ';s++,len--);
    else
      for (;len && *(s+len-1)==' ';len--);    
  }
  return len;
}

char *dbf_describe_db (unsigned char type)
{
  int i=sizeof(DB_TYPE)/sizeof(db_type);
  for (;i;i--)
    if (DB_TYPE[i-1].type==type)
      return DB_TYPE[i-1].long_des;

  fprintf (stderr,"Warning: `0x%02X` type not found!\n",type);
  return NULL;  
}
char *dbf_describe_field_type (char type)
{
  int i=sizeof(FIELD_TYPE)/sizeof(field_type);
  for (;i;i--)
    if (FIELD_TYPE[i-1].type==type)
      return FIELD_TYPE[i-1].long_des;

  fprintf (stderr,"Warning: `%c` type not found!\n",type);
  return NULL;  
}

void dbf_showinfoheader (DBF_HEADER *header)
{
    printf ("+------------------------------------------+\n");
    printf ("| DataBase type: 0x%02X   Update: 20%02d-%02d-%02d |\n",header->FileType,header->LastUpdate[0],header->LastUpdate[1],header->LastUpdate[2]);
    printf ("| %s       |\n",dbf_describe_db(header->FileType));
    printf ("+------------+-------------+---------------+\n");
    printf ("| Number of  | Position of | Len of Record |\n");
    printf ("| Records    | 1st  Record | w delete flag |\n");
    printf ("| %10d | %11d | %13d |\n",header->NumberOfRecords,header->PositionOfFirstDataRecord,header->LengthOfOneDataRecord);
    printf ("+------------+-------------+---------------+\n");
}

void dbf_show_infofields (DBF *dbf)
{
  // FIXME
  int i;
  static char FieldName[11];
  
  dbf_showinfoheader (dbf->header);
  
  printf ("+------------+-------------+-------+-------+\n");
  printf ("| Field Name | Type        | Len   | Dec   |\n");
  printf ("+------------+-------------+-------+-------+\n");
  for (i=0;i<dbf->NumberOfFields;i++)
  {
    memset (FieldName,' ',sizeof(FieldName)-1); FieldName[sizeof(FieldName)-1]=0;
    memcpy (FieldName,dbf->sub_header[i]->FieldName,strlen(dbf->sub_header[i]->FieldName));
    printf ("| %10s | %c %s | %5d | %5d |\n",
            FieldName,
            dbf->sub_header[i]->FieldType,
            dbf_describe_field_type(dbf->sub_header[i]->FieldType),
            LengthOfField(dbf->sub_header[i]),
            NumberOfDecimalPlaces(dbf->sub_header[i])
           );
  }
  printf ("+------------+-------------+-------+-------+\n");
}

void dbf_push_record (DBF *dbf, int r, FILE *fp)
{
  int k,i;
  ui32 position;
  ui16 len;
  
  if (dbf==NULL)
    return;
  
  fprintf (fp,"%09d\tx",r);
  for (k=0;k<dbf->norder;k++)
  {
    i=dbf->order[k];
    memcpy (&position,dbf->sub_header[i]->DisplacementOfFieldInRecord,sizeof(position));
    len=LengthOfField(dbf->sub_header[i]);
    memcpy (dbf->tmp_sub_record,dbf->record+position,len); dbf->tmp_sub_record[len]=0;
    
    if (_dbf_isnumeric(dbf->sub_header[i]))
      numstr_to_pad0 (dbf->tmp_sub_record);
    
    fprintf (fp,"\t%s",dbf->tmp_sub_record);
  }
  fputc ('\n',fp);
  
  // Setting RealLengthOfField in select Field(s) 
  for (k=0;k<dbf->nselect;k++)
  {
    i=dbf->select[k];
    
    memcpy (&position,dbf->sub_header[i]->DisplacementOfFieldInRecord,sizeof(position));
    len=LengthOfField(dbf->sub_header[i]);
    memcpy (dbf->tmp_sub_record,dbf->record+position,len); dbf->tmp_sub_record[len]=0;
    
    if (dbf->sub_header[i]->RealLengthOfField<real_len(dbf->tmp_sub_record,dbf->sub_header[i]->FieldType))
        dbf->sub_header[i]->RealLengthOfField=real_len(dbf->tmp_sub_record,dbf->sub_header[i]->FieldType);
  }
   
}

int dbf_dump (DBF *dbf,FILE *fp, char *tmpname)
{
  int i,err=0;
  ui32 RecNumber=0; 
  char x[16];
  
  dbf->dbf_dump_HEAD (dbf);
  while (!err && !feof(fp))
  {
    if (fscanf (fp,"%15s %*[^\n]",x)>=0)
    {
      i=atoi(x);
      if ((err=_dbf_load_record (dbf->fp_in, dbf->header, dbf->record, i, dbf->header->LengthOfOneDataRecord))==0)
      {
        RecNumber++;
        dbf->dbf_dump_BODY (dbf,RecNumber);
      }
    }
  }
  dbf->dbf_dump_TAIL (dbf);
  pclose(fp);
  if (unlink(tmpname)<0)
    perror ("unlink");
  
  return err;
}

int dbf_show_records (DBF *dbf)
{
  int i,err=0;
  double cmp;
  int len;
  ui32 position;
  CONDITION *p;
  
  char cmd[256];
  char tmpname [L_tmpnam];
  FILE *fp;
  
  /* Set var_id */
  for (p=dbf->condition;p;p=p->next)
  {
    for (i=0;i<dbf->NumberOfFields;i++)
    {
      if ((xstrcmpi (dbf->sub_header[i]->FieldName,p->var))==0)
      {
        p->var_id=i;
        break;
      }
    }
    if (i==dbf->NumberOfFields)
    {
      printf ("field \'%s\' not found in archive dbf\n",p->var);
      err=-1;
    }
  }
  if (err)
    return err;


  if ((mkstemp(tmpname))<0)
  {
    perror ("mkstemp");
    return -1;
  }
  if ((fp=fopen (tmpname,"w"))==NULL)
  {
    printf ("Cannot open `%s`\n",tmpname);
    perror ("");
    return -1;
  }
    
  /* Applying conditions */
  dbf->NumberOfRecords=0;
  for (i=1;i<=dbf->header->NumberOfRecords;i++)
  {
    _dbf_load_record (dbf->fp_in, dbf->header, dbf->record, i, dbf->header->LengthOfOneDataRecord);
    if (_dbf_isdelete(dbf->record)>0)
      continue;
    
    p=dbf->condition;
    
    while (p)
    {
      position=char2ui32(dbf->sub_header[p->var_id]->DisplacementOfFieldInRecord);
      len=LengthOfField(dbf->sub_header[p->var_id]);
      if (_dbf_isnumeric(dbf->sub_header[p->var_id]))
        cmp=(strtod(dbf->record+position,NULL) - strtod(p->val,NULL));
      else
        cmp=(double)strncmp (dbf->record+position,p->val,strlen(p->val));
    
      if ( (p->op_id==EQ && cmp==0) || (p->op_id==LT && cmp<0) || 
           (p->op_id==LE && cmp<=0) || (p->op_id==GT && cmp>0) || 
           (p->op_id==GE && cmp>=0) || (p->op_id==NE && cmp!=0) )
        p=p->next;     
      else
        break;
      
    }
    if (p==NULL)
    {
      dbf->NumberOfRecords++;
      dbf_push_record (dbf, i, fp);
    }
  }
  fclose (fp);
  
  if (dbf->NumberOfRecords==0)
  {
    printf ("Record not found!\n");
    return 0;
  }

  sprintf (cmd,"LC_COLLATE=C sort %s -k2",tmpname);
  fp=popen (cmd, "r");
 
  err=dbf_dump (dbf,fp,tmpname);

  return err;
        
}

int dbf_init (DBF **dbf, char *filename)
{
  DBF *new;
  int i;
  ui16 MaxLenghtField=0;
  
  if ((new=(DBF *) xmalloc (sizeof(DBF)))==NULL)
  {
    perror ("xmalloc");
    return -1;
  }
  new->fp_in=NULL;
  new->database=filename;
  new->condition=NULL;
  new->select=NULL;
  new->nselect=0;
  new->order=NULL;
  new->norder=0;
  new->tmp_sub_record=NULL;

  if ((new->fp_in=fopen(filename,"rb"))==NULL)
  {
    fprintf (stderr,"Error: Cannot open `%s`\n",filename);
    perror ("fopen");
    free (new);
    return -1;
  }
  
  // Remove Pathname and extention
  xbasename(new->database,strlen(new->database));
  
  if ((_dbf_init (new->fp_in,&new->header,&new->sub_header, &new->NumberOfFields, &new->record))!=0)
  {
    fclose (new->fp_in);
    free (new);
    return -1;
  }

  // Alloc TMP
  for (i=0;i<new->NumberOfFields;i++)
    if (LengthOfField(new->sub_header[i])>MaxLenghtField)
      MaxLenghtField=LengthOfField(new->sub_header[i]);
  if ((new->tmp_sub_record=(char *)xmalloc (MaxLenghtField+1))==NULL)
  {
    perror ("xmalloc");
    return -1;
  }
  
  *dbf=new;
  
  return 0;
}
void dbf_end (DBF *dbf)
{
  if (dbf)
  {
    _dbf_end (dbf->header, dbf->sub_header, dbf->NumberOfFields, dbf->record);
    
    free_conditions (dbf->condition);
    if (dbf->select && dbf->nselect)
    {
      free_select (dbf->select);
      dbf->nselect=0;
    }
    if (dbf->order && dbf->norder)
    {
      free_order (dbf->order);
      dbf->norder=0;
    }
    if (dbf->tmp_sub_record);
      free (dbf->tmp_sub_record);
    
    fclose (dbf->fp_in);
    free (dbf);
  } 
}

