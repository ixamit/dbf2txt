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

int main (int argc, char **argv)
{
  DBF *dbf=NULL;
  int err=0;
  char *options_line=NULL,*xdump=NULL;
  char *arch;

  LIST_PLUGIN *plugin_head=NULL, *plugin;
  load_all_plugins (&plugin_head);
 
  if (opt_init (argc,argv,&options_line)>0)
  {
    // the main option parser 
    if ((err=opt_index (options_line))<0)
      ;
    
    if (options_line) xfree (options_line);    
  }
  if (opt_get("help")) 
  {
    opt_free ();
    return 0;
  }

  ///////////////////////////////////////////////
  // xdump Setting 
  //
  void dbf_dump_ASCII_HEAD (DBF *);
  void dbf_dump_ASCII_TAIL (DBF *);
  void dbf_dump_ASCII_BODY (DBF *, ui32);
	
  void *HEAD=dbf_dump_ASCII_HEAD;
  void *BODY=dbf_dump_ASCII_BODY;
  void *TAIL=dbf_dump_ASCII_TAIL;
  if (!err) 
  {
	if ((xdump=opt_get("xdump")))
	{
		if (!xstrcmpi(xdump,"HELP"))
		{
			show_plugin_list (plugin_head);
			return 0;
		}
		if ((plugin=search_plugin (plugin_head, xdump))==NULL)
		{
			fprintf (stderr,"Error: Invalid plugin `%s` in --xdump. \n",xdump);
			err=-1;
		}
		else
		{
			HEAD=plugin->plugin->HEAD;
			TAIL=plugin->plugin->TAIL;
			BODY=plugin->plugin->BODY;
		}
	}
  }
  ///////////////////////////////////////////////
  
  if (opt_get_argc())
  {
    fprintf (stderr,"Error: `%s` invalid parameters. (I think you be mistaken with option)\n",opt_get_argv(0));
    err=-1;
  }
  if (!err && !opt_get("from") && !opt_get("desc")) 
  {
    fprintf (stderr,"Error: required option `--desc <archive.dbf>` or `--from <archive.dbf>`\n");
    opt_usage();
    err=-1;
  }
  if ((arch=opt_get("desc"))==NULL) 
    arch=opt_get("from");
 
 
  if (!err && (err=dbf_init (&dbf,arch))<0)
    opt_usage();
  if (!err)
  {
  	// Set xdump plugin into dbf struct
	dbf->dbf_dump_HEAD=HEAD;
	dbf->dbf_dump_TAIL=TAIL;
    dbf->dbf_dump_BODY=BODY;
  }
 
  if (!err && opt_get("desc"))
  {
    dbf_show_infofields (dbf);
    dbf_end (dbf);
    opt_free ();
    return 0;
  }

  
  if (!err && (err=make_select (opt_get("select"), dbf))<0)
    ;

  if (!err && opt_get("where"))
  {
    if ((err=make_condition (opt_get("where"), &dbf->condition))<0)
      ;
  }
  if (!err && opt_get("order"))
  {
    if ((err=make_order (opt_get("order"), dbf))<0)
      ;
  }
  
  if (!err)
    // 
    dbf_show_records (dbf);
    //
  
  dbf_end (dbf);
  opt_free ();
  
  return 0;

}
