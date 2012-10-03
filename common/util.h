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
#ifndef _UTIL_H
#define _UTIL_H

int base2int (int base,char *string);
int putnc (int character, int n, FILE * stream);
int ltrim (char *str);
int rtrim (char *str);
int alltrim (char *str);
int xstrcmpi  (const char *s1, const char *s2);
int xstrncmpi  (const char *s1, const char *s2, size_t num);
char *numstr_to_pad0(char *s);
char *pad0_to_numstr(char *s);
char *xbasename (char *pathname, size_t len);
#endif // _UTIL_H