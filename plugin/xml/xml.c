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

///////////////////////////////////////////////////////////////////////////////
//
// Module XML ver 0.1
//
///////////////////////////////////////////////////////////////////////////////

#include "xdump.h"

int plugin_init (PLUGIN *xml)
{
	xml->name="XML";
	xml->desc="Extensible Markup Language (XML)";
	xml->vers="0.1";

	return 0;
}

void XML_HEAD (DBF *dbf)
{
	printf ("<?xml version=\"1.0\"?>\n");
	printf ("<%s xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n",PACKAGE);
	printf ("  <database name=\"%s\">\n","dummy");
	printf ("    <table_structure name=\"%s\">\n",DBNAME);
	// loop
	FOREACH_SELECT
	{
		printf ("      <field Field=\"%s\" Type=\"",DBFIELD_NAME(i));
		switch (DBFIELD_TYPE(i))
		{
			case 'N':
      			if (DBFIELD_DEC(i))
					printf ("decimal(%d,%d)",DBFIELD_LEN(i),DBFIELD_DEC(i));
				else if (DBFIELD_LEN(i)<5)
					printf ("smallint(%d)",DBFIELD_LEN(i));
				else if (DBFIELD_LEN(i)<3)
					printf ("tinyint(%d)",DBFIELD_LEN(i));
				else
					printf ("int(%d)",DBFIELD_LEN(i));
				break;
			case 'C':
				if (DBFIELD_LEN(i)<=255)
					printf ("varchar(%d)",DBFIELD_LEN(i));
				else
					printf ("text(%d)",DBFIELD_LEN(i));
				break;
			case 'D':
				printf ("date");
				break;
			default:
				printf ("Unknow");
				break;
		}
		printf ("\" />\n");	
	}
	//
	printf ("    </table_structure>\n");
	printf ("    <table_data name=\"%s\">\n",DBNAME);
}

void XML_BODY (DBF *dbf,ui32 RecNumber)
{
	printf ("      <row>\n");
	// loop
	FOREACH_SELECT
	{
		DBFIELD_LOAD_CONTENTS(i);
		printf ("        <field name=\"%s\">%s</field>\n",DBFIELD_NAME(i),DBFIELD_CONTENTS);
	}
	//
	printf ("      </row>\n");
}

void XML_TAIL (DBF *dbf)
{
	printf ("    </table_data>\n");  
	printf ("  </database>\n");
	printf ("</%s>\n",PACKAGE);
}


