/**
  STANDARD INPUT LIBRARY

  Copyright (C) 2012, Max Cavallo <ixamit_at_gmail_dot_com>
 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
  --------------------------------------------------------------------------
  2011/02/27 *nix    tested 32/64 bits
  2011/02/27 Windows tested           - thx skynet at iprogrammatori.it
  2011/08/26- add in_option
  BUG
  2011/02/27- fixed size_t to int     - thx skynet at iprogrammatori.it
  2011/02/27- fixed tail extra space  - thx skynet at iprogrammatori.it
  2011/07/28- fixed fflush in in_char - ixamit at gmail dot com
  --------------------------------------------------------------------------
  2011/02/27 valgrind tested
  ==15498==
  ==15498== HEAP SUMMARY:
  ==15498==     in use at exit: 0 bytes in 0 blocks
  ==15498==   total heap usage: 4,376 allocs, 4,376 frees, 153,020,616 bytes allocated
  ==15498==
  ==15498== All heap blocks were freed -- no leaks are possible
  --------------------------------------------------------------------------

  int in_string (FILE *stream,char **string, int max_size)
  Read string from stream allocating CHUNK space.
  Allow unknows dynamic space w/o garbage problems
  input : 
     stream     the streamer
     string     reference to pointer buffer
     max_size   if set to zero it allocs CHUNK dynamic spaces
  return:
     IN_ERROR or NULL ptr string when error occours. you can use either ferror or feof to determinate result.


  int in_value (FILE *stream, const char *format,...)
  Read value(s) from stream using in_string + sscanf
  input : 
     stream     the streamer
     format     according to sscanf format
     ...        sequence of reference arguments
  return:   
    referenced arguments or IN_ERROR when error occours 


  int in_char   ( FILE *stream );
  Read one char from streamer
  input : 
     stream     the streamer
  return:
     char as an int. EOF or error
     
     
  int in_option (FILE *stream, char *str, int type)
  perform in_string searching it into word list
  input : 
     stream     the streamer
     str        word list separated by space or comma
     type       case sensive search. IN_NOCS || IN_CS 
  return:   
     1 to n, zero not maching result || error
    
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <in.h>

int _in_char (FILE *stream)
{
  return (fgetc(stream));
}

int in_char (FILE *stream)
{
  int ch;
  ch=_in_char(stream);
  
  if (ch!='\n')
    while (_in_char(stream)!='\n');
  
  return ch;
}

int in_string (FILE *stream, char **string, int max_size)
{
  int c;
  int len,size;
  char *s,*n;
  
  size=(max_size) ? max_size : IN_CHUNK_LEN;
  if ((s=(char *)malloc ((size_t)size))==NULL)
  {
    *string=NULL;
    return IN_ERROR;
  }

  for (len=0;!max_size || len+1<max_size;len++)
  {
    c=_in_char(stream);
    if (c=='\n' || feof(stream) || ferror(stream))
      break;
    
    if (!max_size && (len+1)%IN_CHUNK_LEN==0)
    {
      size+=IN_CHUNK_LEN;
      if ((n=(char *)realloc (s,(size_t) size))==NULL)
      {
        free (s);
        *string=NULL;
        return IN_ERROR;
      }
      s=n;
    }
    s[len]=(char) c;
  }
  s[len]='\0';
  
  if (feof(stream) || ferror(stream))
  {
    free (s);
    s=NULL;
  }
  
  *string=s;
  
  return len;
}

int in_value (FILE *stream, const char *format,...)
{
  static char *values=" .+-0123456789";
  
  char sub_format[16], modifier;
  char *string,*s;

  int len,flush,ExitCode;

  void *ref_ptr;
  
  va_list va;
  
  len=in_string (stream,&string,0);
  while (len && string[len-1]==' ')    string[--len]='\0';
  ExitCode=(len < 1 || string==NULL) ? IN_ERROR : 0;
  for (--len;!ExitCode && len>=0 ;len--)
    if (!strchr (values,string[len]))
        ExitCode=IN_ERROR;
  
  if (ExitCode)
  {
    if (string) free (string);
    return ExitCode;
  }
  
  va_start(va,format);

  len=flush=0; modifier=' '; s=string; 
  while (*format!='\0' && *s!='\0' && len+1<16)
  {
          
    sub_format[len++]=*format;
    
    if (strchr ("hlL",*format))
    {
      //
      // Modifier
      //
      modifier=*format;
      //
    }
    if (strchr("din",*format))
    {
      //
      //  Type  int *
      //
      if (modifier=='h')
        ref_ptr=(short int *)           va_arg(va,short int *);
      if (modifier=='l')
        ref_ptr=(long int *)            va_arg(va,long int *);
      else
        ref_ptr=(int *)va_arg(va,int *);
      
      flush=1;
    }
    if (strchr("oux",*format))
    {
      //
      //  Type  unsigned int *
      //
      if (modifier=='h')
        ref_ptr=(unsigned short *)      va_arg(va,unsigned short *);
      if (modifier=='l')
        ref_ptr=(long unsigned int *)   va_arg(va,long unsigned int *);
      else
        ref_ptr=(unsigned int *)        va_arg(va,unsigned int *);
      
      flush=1;
    }
    if (strchr("efg",*format))    
    {
      //
      // Type   float *  
      //
      if (modifier=='l')
          ref_ptr=(double *)            va_arg(va,double *);
      if (modifier=='L')
          ref_ptr=(long double *)       va_arg(va,long double *);
      else
        ref_ptr=(float *)va_arg(va,float *);
      
      flush=1;
    }
    if (flush)
    {
      sub_format[len++]='\0';
      if ((ExitCode=sscanf (s,sub_format, ref_ptr))==IN_ERROR)
        break;
      
      while (*s && *s==' ')  s++;    while (*s && *s!=' ')   s++;
      
      len=flush=0; modifier=' ';
    }
  
    format++;
    
  }
  
  if (*format!='\0' || *s!='\0')
    ExitCode=IN_ERROR;
  
  va_end(va);
  free (string);
  
  return ExitCode;
  
}

