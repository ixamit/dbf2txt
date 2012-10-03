/**
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

  COMMON LIBRARY
  
*/  

#ifndef _IN_H
#define _IN_H
/* Public */
#include <stdio.h>
#include <stdlib.h>

#define IN_CHUNK_LEN   0x10
#define IN_ERROR       -1

int     in_char   ( FILE *stream );
int     in_string ( FILE *stream, char **string, int  max_size );
int     in_value  ( FILE *stream, const char *format, ... );

#endif // _IN_H
