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
#define MAX_ARRAY 1024

/**
    search token into string
    return ptr || NULL
*/
char *compare (char *string, char *token)
{
  int k;
  char *p0=string,*p1;

  if (string==NULL || token==NULL)
    return NULL;

  while (p0)
  {
    p1=strchr (p0,token[0]);
    if (!p1)    
      break;
    
    for (k=1;p1[k] && token[k];k++)
    {
      if (p1[k]!=token[k])
      {
        p0=p1+1;
        p1=NULL;
        break;
      }
    }
    if (!token[k])
      break;
    
    if (p1 && !p1[k])
    {
      p0=p1+1;
      p1=NULL;
    }
  }
  
  return p1;
}

void free_explode (char **table)
{
  if (!table)
    return;
  
  free (*table);
  free (table);
}

char **explode (char *string,char *delimiter, int *num_result)
{
  
  char *p0,*p1,**table;
  int len_delimiter;
  int n=0,size=0;
  int k,j,l;
  
  int tmp[MAX_ARRAY];

  *num_result=0;
  if (string==NULL || delimiter==NULL)
    return NULL;
  
  p0=string;
  len_delimiter=strlen(delimiter);
  
  while (p0 && n+1<MAX_ARRAY)
  {
    p1=compare (p0, delimiter);
    if (p1)
    {
      tmp[n++]=(int)(p1-string);
      p0=p1+1;
    }
    else 
      break;
  }
  tmp[n++]=strlen(string);
  
  /**
  * Calculate the size then
  * Allocate block && table
  */
  for (size=tmp[0]+1,k=1;k<n;k++)
    size+=(tmp[k]-tmp[k-1]-len_delimiter+1);
  
  if ((p1=malloc (size*sizeof(char)))==NULL)
  {
    printf ("Cannot allocate in explode\n");
    return NULL;
  }
  if ((table=malloc ((n+1)*sizeof(*table)))==NULL)
  {
    printf ("Cannot allocate in explode\n");
    free (p1);
    return NULL;
  }
  //
  // copying mem && set table
  //
  p0=string;
  for (j=k=0;k<n;k++)
  {
    l=tmp[k]-j;
    if (k)
      l-=len_delimiter;
    
    if (l>0)
      memcpy (p1,p0,l);
    
    p1[l]='\0';
    
    *(table+k)=p1;
    p1+=(l+1);
    p0+=(l+len_delimiter);
    j=tmp[k];
  }
  /**
  * Extra pointer added pointing last '\0'
  * It's used by implode
  */
  *(table+k)=p1-1; 
  
  *num_result=n;
  
  return table;
}

void free_implode (char *string)
{
  if (string)
    free (string);
}

char *implode (char *glue, char **array, int num_result)
{
  char *string;
  int len_glue=strlen(glue); 
  int k,j,i,l,size;

  if (num_result==0)
    return NULL;
  
  size=(array[num_result]-array[0])+(len_glue*(num_result-1));
  
  if ((string=malloc (size*sizeof(char)))==NULL)
  {
    printf ("Cannot allocate in implode\n");
    return NULL;
  }
  
  for (i=j=k=0;k<num_result;k++)
  {
    l=(array[k+1]-array[k]);
    if (k+1<num_result)
      l--;
    
    memcpy (string+i,array[k],l);
    i+=l;
    
    if (k+1<num_result)
    {
      memcpy (string+i,glue,len_glue);
      i+=len_glue;
    }
    
  }
  *(string+i)='\0';
  
  return string;
}
