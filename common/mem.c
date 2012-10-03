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

void xfree (void *p)
{
  if (p)
    free (p);
}
void *xmalloc (size_t size)
{
  void *p;
  if ((p = malloc(size))==NULL)
    perror("malloc failed");

  return p;
}
char *mem2str(char *p, size_t n)
{
  char *q;

  if ((q = xmalloc(n+1))==NULL)
    exit (1);
  memcpy(q, p, n+1);
  *(q+n)=0;
  return q;
}
