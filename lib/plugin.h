typedef struct _plugin
{
	// public
	char *name;        // pls use 2-5 chars... no take longer
	char *desc;        // describe the plugin
	char *vers;        // ::

    // private
	char *pathname;    // library	
	void *handle;      // handle to shared lib
	void *HEAD;
	void *BODY;
	void *TAIL;
	
    
	/*
		Will use [name]_HEAD, [name]_BODY and [name]TAIL
		as target symbol in the dump option.

	*/
	
}PLUGIN;

typedef struct _list_plugin
{
	PLUGIN *plugin;
	struct _list_plugin *next;
}LIST_PLUGIN;

void free_all_plugins (LIST_PLUGIN *head);
int load_all_plugins (LIST_PLUGIN **head);
LIST_PLUGIN *search_plugin (LIST_PLUGIN *head, const char *name);
void show_plugin_list (LIST_PLUGIN *head);
