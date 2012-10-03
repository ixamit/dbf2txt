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
#include <string.h>
#include <ctype.h>

int putnc (int character, int n, FILE * stream)
{
  int r;
  while (n-- && (r=putc (character, stream)));
  
  return r;
}
  
/** 
  trim left string
*/
int ltrim (char *str)
{
  int i,j;
  for (i=0;str[i]!='\0' && (str[i]==' '||str[i]=='\t');i++);
  for (j=0;str[i]!='\0';i++,j++)
    str[j]=str[i];
  str[j]='\0';
  return j;
}
/** 
  trim right string
*/
int rtrim (char *str)
{
  int i;
  for (i=strlen(str)-1;i>=0 && str[i]==' ';i--);
  str[++i]='\0';  
  return i;
}
/** 
  trim right and left string
*/
int alltrim (char *str)
{
  int len;
  ltrim (str);
  len=rtrim (str);
  
  return len;
}

int base2int (int base,char *string)
{
  static char *t="0123456789ABCDEF";
  char *p;
  int l,i,value=0;
  if (!string)  return -1;

  alltrim (string);
  
  l=strlen(string)-1;
  
  for (i=1;l>=0;i*=base,l--)
  {
    if ((p=strchr(t,toupper(string[l])))==NULL)
      return -1;
    value+=(p-t)*i;
  }
  return (value);
}

int xstrcmpi  (const char *s1, const char *s2)
{
  int i,r;
  
  if (!s2 && s1)
    r=s1[0];
  else if (!s1 && s2)
    r=-s2[0];
  else if (!s1 && !s2)
    r=0;
  else
    for (i=0;!(r=toupper(s1[i])-toupper(s2[i]));i++)
      if (!s1[i] || !s2[i])
        break;
    
  return r;
}

int xstrncmpi  (const char *s1, const char *s2, size_t num)
{
  int i,r;
  
  if (!s2 && s1)
    r=s1[0];
  else if (!s1 && s2)
    r=-s2[0];
  else if (!s1 && !s2)
    r=0;
  else
    for (i=0;num && !(r=toupper(s1[i])-toupper(s2[i]));i++,num--)
      if (!s1[i] || !s2[i])
        break;
    
  return r;
}

char *numstr_to_pad0(char *s)
{
  char *r=s;
  if (s)
  {
    while (*s==' ')
      *s++='0';
    
    if (*s=='-')
    {
      *s='0';
      *r='-';
    }
  }
  return r;    
}
char *pad0_to_numstr(char *s)
{
  char *r=s;

  if (s)
  {
    if (*r=='-')
      s++;
    
    while (*s=='0')
      *s++=' ';
    
    if (!*s || *s=='.' || *s==',')
      *(--s)='0';
    if (*r=='-' && --s > r)
    {
      *s='-';
      *r=' ';
    }
  }
  return r;    
}

char *xbasename (char *pathname, size_t len)
{
  // Return basename from pathname
  // without extension
  // Warning: It modify pathname
  char *p;
  int i,j;
  
  if (!pathname)
    return NULL;

  // Remove path
  for (i=len;i;i--)
  {
    p=pathname+i-1;
    if (*p=='/')
    {
      for (j=0;i<len;j++,i++)
        *(pathname+j)=*(pathname+i);
      *(pathname+j)=(char) 0;
      len-=(i-j);
      break;
    }
  }
  
  // Remove extension
  for (i=len;i>1;i--)
  {
    p=pathname+i-1;
    if (*p=='.' && isalnum(*(p+1)))
    {
      *p=(char)0;
      break;
    }
  }
  return pathname;
}

