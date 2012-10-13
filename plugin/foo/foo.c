/**
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

///////////////////////////////////////////////////////////////////////////////
//
// Module FOO ver 0.1
//
///////////////////////////////////////////////////////////////////////////////

#include "xdump.h"

int plugin_init (PLUGIN *foo)
{
	foo->name="FOO";
	foo->desc="File or Object (FOO)";
	foo->vers="0.1";

	return 0;
}

void FOO_HEAD (DBF *dbf)
{
    printf ("Foo Head\n");
}

void FOO_TAIL (DBF *dbf)
{
  printf ("Foo Tail\n");
}

void FOO_BODY (DBF *dbf,ui32 RecNumber)
{
  printf ("Foo Body - RecNumber %u\n",RecNumber);
}

