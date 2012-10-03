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
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <dlfcn.h>

#include "mem.h"
#include "plugin.h"

void show_plugin_list (LIST_PLUGIN *head)
{
	printf ("Available xdump plugins:\n");
    printf ("name:\tdescription: [lib]\n");
	for (;head;head=head->next)
	{
		printf ("%s\t",head->plugin->name);
		printf ("%s\t",head->plugin->desc);
		//printf ("vers=%s\n",head->plugin->vers);
		//printf ("handle=%p\n",head->plugin->handle);
		printf ("\n");
	}
}

LIST_PLUGIN *search_plugin (LIST_PLUGIN *head, const char *name)
{
	for (;head;head=head->next)
		if ((xstrcmpi (head->plugin->name,name))==0)
			return head;
	return NULL;
}

void free_all_plugins (LIST_PLUGIN *head)
{
	LIST_PLUGIN *next;
	for (;head;head=next)
	{
		dlclose(head->plugin->handle);
		free(head->plugin);
		next=head->next;
		free(head);
	}
}
int push_plugin (LIST_PLUGIN **head, const PLUGIN *plugin)
{
	PLUGIN *new_plugin=(PLUGIN *)xmalloc (sizeof(PLUGIN));
	LIST_PLUGIN *new_list=(LIST_PLUGIN *)xmalloc (sizeof(LIST_PLUGIN));

	if (new_plugin==NULL | new_list==NULL)
	{
		if (new_plugin) free (new_plugin);
		if (new_list)   free (new_list);
		return -1;
	}

	memcpy (new_plugin, plugin, sizeof(PLUGIN));
	new_list->plugin=new_plugin;
	new_list->next=*head;
	*head=new_list;

    return 0;	
}

int load_plugin (LIST_PLUGIN **head, const char *lib)
{
	void *handle;
	char *pathname;
	size_t len;
    int (*check_init)(PLUGIN *);
	char *error;
	PLUGIN plugin;

	// I'm paranoic	
	if (!lib) return -1;

	len=sizeof(LIBDIR)+strlen(lib)+2;
	if ((pathname=xmalloc (len))==NULL)
		return -1;

	snprintf (pathname,len,"%s/%s",LIBDIR, lib);
	handle = dlopen (pathname, RTLD_LAZY);
	free (pathname);
	if (!handle)
	{
		fprintf (stderr, "%s\n", dlerror());
		return -1;
	}
	
	dlerror(); // Clear any existing error 

	*(void **) (&check_init) = dlsym(handle, "plugin_init");

	if ((error = dlerror()) != NULL)  
	{
		//fprintf(stderr, "%s\n", error);
		dlclose(handle);
        return -1;
	}
	
	(*check_init)(&plugin);

	if (!plugin.name)
	{
		fprintf (stderr,"Warning Plugin `%s`: no name definition\n", lib);
		dlclose(handle);
		return -1;

	}
	if (search_plugin (*head, plugin.name))
	{
		fprintf (stderr,"Warning Plugin `%s`: redefinition of name `%s`\n",lib,plugin.name);
		dlclose(handle);
		return -1;
	}

	
	char *func_name=xmalloc(strlen(plugin.name)+6);
	if (!func_name)
	{
		dlclose(handle);
		return -1;
	} 
	sprintf (func_name,"%s_%s",plugin.name,"HEAD");
	if ((plugin.HEAD=dlsym(handle, func_name))==NULL)
	{
		fprintf (stderr,"Warning Plugin `%s`: `%s` not found\n", lib,func_name );
        dlclose(handle);
        return -1;
	}
	sprintf (func_name,"%s_%s",plugin.name,"BODY");
	if ((plugin.BODY=dlsym(handle, func_name))==NULL)
	{
		fprintf (stderr,"Warning Plugin `%s`: `%s` not found\n", lib,func_name );
        dlclose(handle);
        return -1;
	}

	sprintf (func_name,"%s_%s",plugin.name,"TAIL");
	if ((plugin.TAIL=dlsym(handle, func_name))==NULL)
	{
		fprintf (stderr,"Warning Plugin `%s`: `%s` not found\n", lib,func_name );
        dlclose(handle);
        return -1;
	}
	
	plugin.handle=handle;
	push_plugin (head, &plugin);

	return 0;
}

int filter (const struct dirent *file)
{
	char *est;

	// skip if not device&regular
	if (file->d_type != (DT_REG|DT_CHR))
		return 0;
	// skip file with no extension
	if ((est=strrchr(file->d_name,'.'))==NULL)
		return 0;
	// skip if not shared lib
    if (strcmp(est+1,"so"))
		return 0;
	
	return 1;
}

int load_all_plugins (LIST_PLUGIN **head)
{
	struct dirent **files;
	int i,n;

	if ((n=scandir(LIBDIR,&files, filter, alphasort))<0)
	{
		fprintf (stderr,"no plugin found in `%s`\n",LIBDIR);
		return -1;
	}
	while (n--)
	{
		load_plugin(head,files[n]->d_name);
		free(files[n]);
	}
	free (files);
	return 0;
}
#if 0
int main ()
{
	LIST_PLUGIN *head=NULL;
    load_all_plugins (&head);

	printf ("XML=%p\n",search_plugin (head,"XMLz"));
	show_plugin_list (head);
	free_all_plugins (head);
	return 0;
}
#endif

