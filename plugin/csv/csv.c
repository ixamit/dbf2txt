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
// Module CSV ver 0.1
//
///////////////////////////////////////////////////////////////////////////////

#include <locale.h>
#include "xdump.h"

extern int CSV_SEPARATOR;
extern int CSV_DECIMAL_POINT;
extern int DECIMAL_POINT_CHANGED;

char *change_decimal_point (char *value)
{
	char *p;
	if (!value) return;
	if ((p=strchr(value,'.')))
		*p=(char)CSV_DECIMAL_POINT;
	return value;
}

int plugin_init (PLUGIN *CSV)
{
	CSV->name="CSV";
	CSV->desc="Comma-Separated Value (CSV)";
	CSV->vers="0.1";

	return 0;
}

void CSV_HEAD (DBF *dbf)
{
	struct lconv *plconv;
    CSV_SEPARATOR=',';
    CSV_DECIMAL_POINT='.';
    DECIMAL_POINT_CHANGED=0;
	setlocale(LC_ALL,"");
	plconv=localeconv ();
	CSV_DECIMAL_POINT=(int) *plconv->decimal_point;
	if (CSV_DECIMAL_POINT==CSV_SEPARATOR)
	{
		CSV_SEPARATOR=';';
		DECIMAL_POINT_CHANGED=1;
	}
	// loop
	int i;
	FOREACH_SELECT(i)
	{
		printf ("\"%s\"",DBFIELD_NAME(i));
		if (INDEXOF+1 < NSELECT)
			printf ("%c",CSV_SEPARATOR);

	}
	//
	printf ("\n");
	
}

void CSV_TAIL (DBF *dbf)
{

}

void CSV_BODY (DBF *dbf,ui32 RecNumber)
{
	// loop
	int i;
	FOREACH_SELECT(i)
	{
		DBFIELD_LOAD_CONTENTS(i);
		if (DBFIELD_ISNUMERIC(i))
			printf ("%s",change_decimal_point(DBFIELD_CONTENTS));
		else
			printf ("\"%s\"",DBFIELD_CONTENTS);
		if (INDEXOF+1 < NSELECT)
			printf ("%c",CSV_SEPARATOR);
	}
	//
	printf ("\n");

}

