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
//
//
///////////////////////////////////////////////////////////////////////////////

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

#include "plugin.h"

#define DEBUG 1

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

void FOO_BODY (DBF *dbf,ui31 RecNumber)
{
  printf ("Foo Body - RecNumber %d\n",RecNumber);
}

