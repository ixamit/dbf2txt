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
// Module HTML ver 0.1
//
///////////////////////////////////////////////////////////////////////////////

#include "xdump.h"

int plugin_init (PLUGIN *html)
{
	html->name="HTML";
	html->desc="HyperText Markup Language (HTML)";
	html->vers="0.1";

	return 0;
}

void HTML_HEAD (DBF *dbf)
{
	// loop
	printf ("<table border=\"1\">\n");
	printf ("\t<tr>\n");
	int i;
	FOREACH_SELECT(i)
	{
		printf ("\t\t<th>%s</th>\n",DBFIELD_NAME(i));
	}
	//
	printf ("\t</tr>\n");

}

void HTML_TAIL (DBF *dbf)
{
	printf ("</table>\n");
}

void HTML_BODY (DBF *dbf,ui32 RecNumber)
{

	printf ("\t<tr>\n");
	// loop
	int i;
	FOREACH_SELECT(i)
	{
		DBFIELD_LOAD_CONTENTS(i);
		printf ("\t\t<td>%s</td>\n",DBFIELD_CONTENTS);
	}
	//
	printf ("\t</tr>\n");

}

