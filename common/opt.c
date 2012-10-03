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

  opt process a command-line. Every option has long and short form. 
  mix and join different option styles like:
    opt -bcd
    opt -b -c -d
    opt --background -c --debug
    opt -c--debug --background
    ...
    opt ARGV -olog
    opt ARGV -o log
    opt --log-file=log ARGV
    opt ARGV --log-file = log
    ...
    opt --execute-command:system -db argv1 --tries  20
    opt --tries='20' -dbe system argv1
    ...
    opt -bcd -t 3 --dns-timeout=10 --retry-connrefused -o log localhost -e dir

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "util.h"
#include "explode.h"
#include "in.h"

#ifdef _WIN32
#ifndef snprintf
#define snprintf _snprintf
#endif
#endif

#ifndef xstrdup
#define xstrdup strdup
#endif
#ifndef xmalloc
#define xmalloc malloc
#endif

#define countof(x) (sizeof (x) / sizeof ((x)[0]))
#define OPT_SPACE     '\40'
#define OPT_SEPARATOR '\43'//'\35'

#define OPT_UNRECOGNIZED_OPTION     "unrecognized option"
#define OPT_REQUIRED_ARGUMENT       "option requires an argument"
#define OPT_NOT_REQUIRED_ARGUMENT   "option do not accept argument"
#define OPT_INVALID_PARAMETER       "invalid parameter"
#define OPT_INVALID_ASSIGNEMENT     "invalid assignement"
#define OPT_OUT_OF_MEMORY           "out of memory"
            
static struct _opt_var *opt_first_var=NULL;
static struct _opt_var *opt_last_var =NULL;

void opt_help ();
void opt_show_config();

struct _opt_var
{
  char *key;
  char *value;
  struct _opt_var *next;
};
struct _opt
{
  /**
    long && short names are refered to option name which called 
    from the command line. prefix are respectivly '--' & '-'
  */
  char *long_name;
  char short_name;              
  
  /**
    assignement char per value. It is used by enum type.
    refered to OPT_ASSIGN
  */
  #define OPT_ASSIGN "= :\0"
  enum {
    opt_equal,
    opt_space,
    opt_colon,
    opt_null,   
  } assign;
  
  /**
    meaning 'type_of_data'.
    opt_bool     == boolean (assign == opt_null)
    opt_value    == numeric (assign == opt_equal | opt_space | opt_colon)
    opt_string   == alpha   (assign == opt_equal | opt_space | opt_colon)
    opt_callback == function pointer callback (opt_null)
  */
  enum {
    opt_callback,
    opt_bool,
    opt_value,
    opt_string,
  } type;
  
  /**
    function pointer callback or NULL
  */
  void *data;
  
  /**
    short help descriptive
    if defined NULL do not display in automatic help
  */
  char *hlp_des;
  
  /**
    extra argument assignement help, ie:
    {"tries",'t' ...... "number"} 
    it will show:
    -t, --tries=number ......
    *used only for automatic helper-
  */
  char *hlp_arg;

};


/**
  =============================================
  put your options arguments here; you need it!
  =============================================
*/
#define OPT_PRG_NAME "dbf2txt"
#define OPT_CONFIGFILE "/etc/" OPT_PRG_NAME "/config"
static char opt_synopsis[]="Usage: " OPT_PRG_NAME " [options]... ";
static struct _opt opt_options[]=
{
  {NULL,0,opt_null,opt_bool,NULL,"Help and Configuration:",NULL},
  {"help",'h',opt_null,opt_callback,(void *)opt_help,"Print this help",NULL},

  {NULL,0,opt_null,opt_bool,NULL,"Query (SQL Style):",NULL},
  {"desc",'d',opt_equal,opt_string,NULL,"Describe an DBF (--desc \"archive.dbf\")","\"ARCHIVE.DBF\""},
  {"select",'s',opt_equal,opt_string,NULL,"Select data field(s) (--select \"FIELD1 FIELD3 FIELD7\")","\"FIELD...\""},
  {"from",'f',opt_equal,opt_string,NULL,"Archive DBF (--form \"archive.dbf\")","\"ARCHIVE.DBF\""},
  {"where",'w',opt_equal,opt_string,NULL,"Condition filter clause. (--where \"FIELD1>=AB FIELD5<100 FIELD9!=' xx ' \")","\"CONDITION...\""},  
  {"order",'o',opt_equal,opt_string,NULL,"Sort result field(s) (--order \"FIELD3 FIELD5 FIELD1\")","\"FIELD...\""},
  {NULL,0,opt_null,opt_bool,NULL,"Output (optional plugins):",NULL},
  {"xdump",'x',opt_equal,opt_string,NULL,"Dump to plugin output. Use `HELP` as NAME for a list of loaded plugins","\"NAME\""},
  
};
/**
  =============================================
*/

void opt_usage ()
{
  printf ("%s\n\n",opt_synopsis);
  printf ("Try `%s --help` for more options.\n", OPT_PRG_NAME);
  
}

/** 
  This is the automatic default debug. 
*/
void opt_show_config ()
{
  int i;
  char *dataname,*value;
  char *opt_get (char *key);

  printf ("\nOptions Stored in %s:\n",OPT_CONFIGFILE);
  for (i=0;i<countof(opt_options);i++)
  {
    dataname=(char *)opt_options[i].long_name;
      if ((value=opt_get(dataname)))
        printf (" --%s=%s\n",dataname,value);
  }
  printf ("\n");
}

/** 
  This is the automatic default help. 
  It's generated by reading the struct opt_options[]; you should easly use that function 
  or different by changing pointer in the help field declared above.
*/

void opt_help ()
{
  char buff[40];
  int i,j;
  
  printf ("%s\n\n",opt_synopsis);
  for (i=0;i<countof(opt_options);i++)
  {
	if (opt_options[i].short_name==0 && opt_options[i].long_name==NULL)
	  printf (" %s\n",opt_options[i].hlp_des);
    else if (opt_options[i].hlp_des)
    {
      snprintf (buff,sizeof(buff)," %c%c%c --%s%c%s",
               (opt_options[i].short_name==' ' || opt_options[i].short_name==0) ? ' ' : '-',
               (opt_options[i].short_name==' ' || opt_options[i].short_name==0) ? ' ' : opt_options[i].short_name,
               (opt_options[i].short_name==' ' || opt_options[i].short_name==0) ? ' ' : ',',
               opt_options[i].long_name,
               OPT_ASSIGN[opt_options[i].assign],
               opt_options[i].hlp_arg);
      printf ("%s",buff);
      for (j=strlen(buff);j<sizeof(buff);j++)
        printf (" ");
      printf ("%s\n",opt_options[i].hlp_des);
    }
  }
  printf ("\n");
}

/**
  dump data to stream
*/
void opt_out (FILE *stream,char *p, int n)
{
  for (;n;n--)
    fprintf (stream,"%c",*(p++));
}

/**
  This is init function. 
  It return an allocated opt string with all argv separated by OPT_SEPARATOR
  ie:  foo bar 'foo bar' ---> foo#bar#foo bar\0
  return word's number or error
*/
void cleaning_argv  (int argc, char **argv)
{
  int i,j,l;
  
  for (i=1;i<argc;i++)
  {
    l=strlen(argv[i]);
    for (j=0;j<l;j++)
      if (!isprint (argv[i][j]))
        memcpy (&argv[i][j],&argv[i][j+1],l-j);
  }
}
int opt_init (int argc,char **argv,char **opt)
{
  int i,err=0,l=0,wc=argc-1;

  opt_first_var=opt_last_var=NULL;
  *opt=NULL;
  
  /** patch cleaning argv */
  cleaning_argv(argc,argv);
  
  /** possibly incongruences type assignement */
  for (i=0;!err && i<countof(opt_options);i++)
  {
    switch (opt_options[i].type)
    {
      case opt_callback:
        if (opt_options[i].assign != opt_null)
          err=opt_options[i].type+1;
        break;
      case opt_bool:
        if (opt_options[i].assign != opt_null)
          err=opt_options[i].type+1;
        break;       
      case opt_value:
        if (opt_options[i].assign != opt_equal && 
            opt_options[i].assign != opt_space &&
            opt_options[i].assign != opt_colon)
          err=opt_options[i].type+1;
        break;
      case opt_string:
        if (opt_options[i].assign != opt_equal && 
            opt_options[i].assign != opt_space &&
            opt_options[i].assign != opt_colon)
          err=opt_options[i].type+1;
        break;       
    }
  }
  if (err)
  {
    fprintf (stderr,"opt_init::opt_options[%d] "OPT_INVALID_ASSIGNEMENT"\n",i);
    return -1;
  }
  
  /**
    TODO more fields control
  */
    
  if (wc)
  {
    for (--argc;argc>0;argc--)
      l+=strlen(argv[argc]);
    
    l+=(wc-1);
    if ((*opt=malloc(l+1))==NULL)
    {
      fprintf (stderr, OPT_OUT_OF_MEMORY"\n");
      return -1;
    }  
    memset (*opt,OPT_SEPARATOR,l); (*opt)[l]='\0';
    
    for (i=0,argc=1;argc<=wc;argc++)
    {
      l=strlen(argv[argc]);
      memcpy (*opt+i,argv[argc],l);
      i+=(l+1);
    }
  }
  
  return (wc);
}

/**
  Just save the key && the value into a linked list.
*/
int set_opt (char *key,char *value, int l_value)
{
  struct _opt_var *node;
  char *x;
  int l_key=(key) ? strlen(key): 0;
  
  /** if key already exist replace it */
  if (key)
  {
    for (node=opt_first_var;node;node=node->next)
    {
      if ((node->key) && (strcmp(node->key,key)==0))
      {
        if ((x=realloc (node->value,l_value+1))==NULL)
        {
          fprintf (stderr,OPT_OUT_OF_MEMORY"\n");
          return -1;
        }
        node->value=x;
        memcpy (node->value,value,l_value);
        node->value[l_value]='\0';
        return 0;
      }
    }
  }
  
  if ((node=(struct _opt_var *)malloc (sizeof(struct _opt_var)))!=NULL)
  {
    node->key=(l_key) ? (char *) malloc (l_key+1) : (char *)0;
    node->value=(l_value) ? (char *) malloc (l_value+1) : (char *)0;
  }
  else
  {
    if (node->value)    free (node->value);
    if (node->key)      free (node->key);
    if (node)           free (node);
    fprintf (stderr,OPT_OUT_OF_MEMORY"\n");
    return -1;
  }
  if (l_key)
    memcpy (node->key, key,l_key+1);
  if (l_value)
  {
    memcpy (node->value,  value, l_value);
    node->value[l_value]='\0';
  }

  node->next=NULL;
  if (!opt_first_var)   opt_first_var=node;
  if (opt_last_var)     opt_last_var->next=node;
  opt_last_var=node;
  
  return 0;
}

/**
  Search opt[ion] in opt_options[]
*/
int opt_search_short (char opt)
{
  int i;
  for (i=0;i<countof(opt_options);i++)
    if (opt_options[i].short_name==opt)
      return i;
    
  fprintf (stderr,OPT_UNRECOGNIZED_OPTION " '-%c'\n",opt);
  return -1;
}
/**
  Search opt[ion] in opt_options[]
*/
int opt_search_long (char *opt)
{
  char *end;
  int i,l;
  
  for (i=0;i<countof(opt_options);i++)
  {
    if (opt_options[i].long_name)
    {
      l=strlen(opt_options[i].long_name);
      if ((strncmp (opt_options[i].long_name,opt,l))==0)
      {
        if (opt[l]==OPT_SEPARATOR || strchr(OPT_ASSIGN,(int)opt[l]))
          return i;
      }
    }
  }
  for (l=0,end=opt;*end && *end!=OPT_SEPARATOR;end++,l++);
  fprintf (stderr,OPT_UNRECOGNIZED_OPTION " '--");
  opt_out (stderr,opt,l);
  printf ("'\n");
  return -1;
}

/**
  Main parsing option_line. 
*/
int opt_index (char *options_line)
{
  char *opt=options_line,*key,*value;
  int is_option,is_option_long, is_option_short;
  int i,index,l_value=0,err=0;
  int (*checkfunc) (int)=NULL;
  
  if (!opt)
    err=-1;
  
  is_option=is_option_long=is_option_short=0;
  while (!err && *opt)
  {
    /**
      init flags 
      removing prefix dash
    */
    index=0; key=value=NULL; l_value=0;
    if (*opt==OPT_SEPARATOR) {is_option=is_option_long=is_option_short=0;opt++;}
    if (*opt=='-') {is_option=1;is_option_short=1;is_option_long=0;opt++;}
    if (*opt=='-') {is_option_long=1;is_option_short=0;opt++;}   


    /** option is short */
    if (is_option_short)
    {
      if ((index=opt_search_short (*opt))>=0)
      {
        key=(char *)opt_options[index].long_name;
        /** move next argument */
        for (++opt;*opt && (*opt==OPT_SEPARATOR || *opt==OPT_SPACE);opt++);
      }
      else
      {
        /** unrecognized short option */
        err=-1;
        break;
      }
    }
    
    /** option is long */
    if (is_option_long)
    {
      if ((index=opt_search_long (opt))>=0)
      {
        key=(char *)opt_options[index].long_name;
        /** move next argument */
        opt+=strlen(opt_options[index].long_name);
        for (;*opt && (*opt==OPT_SEPARATOR || *opt==OPT_SPACE);opt++);
        if (*opt && *opt==OPT_ASSIGN[opt_options[index].assign])
          for (opt++;*opt && (*opt==OPT_SEPARATOR || *opt==OPT_SPACE);opt++);
      }
      else
      {
        /** unrecognized long option */
        err=-1;
        break;
      }
    }

    /** execute callback function */
    if (is_option && opt_options[index].type==opt_callback)
    {
      void (*func) (void) = (void (*) (void)) opt_options[index].data;
      func ();
      /** FIXME */
      key=(char *)opt_options[index].long_name;
      value=NULL;
      //break;
      //continue;
    }

    /** getting value || ARGV */
    if (  !is_option                           ||
          opt_options[index].type==opt_value   || 
          opt_options[index].type==opt_string    )
    {
      value=opt;
      for (l_value=0;*opt && *opt!=OPT_SEPARATOR;opt++,l_value++);
    }
      
    /** check 4 value */  
    if (is_option && value)
    {
      if (l_value==0)
      {
        fprintf (stderr,"\'%s\' "OPT_REQUIRED_ARGUMENT"\n",opt_options[index].long_name);
        err=-1;
        break;
      }
      switch (opt_options[index].type)
      {
        case opt_string:
          checkfunc=isprint;
          break;
        case opt_value:
          checkfunc=isdigit;
          break;
        default:
          break;
      }
      for (i=0;i<l_value;i++)
      {
        if (checkfunc(value[i])==0)
        {
          fprintf (stderr,"\'--%s\' "OPT_INVALID_PARAMETER" \"",opt_options[index].long_name);
          opt_out (stderr,value,l_value);
          printf ("\"\n");
          err=-1;
          break;
        }    
      }     
    }
    
    if (!err)
    {
      err=set_opt (key,value, l_value);
    }
    if (value==NULL && *(opt-1)==OPT_SEPARATOR) opt--;
  }
 
  return err;
}

/**
  Get the ARGC parsed
*/
int opt_get_argc ()
{
  struct _opt_var *p;
  int argc;
  for (argc=0,p=opt_first_var;p;p=p->next)
    if (!p->key) argc++;
  return argc;
}
/**
  Get the ARGV[n] parsed
  return value || NULL
*/
char *opt_get_argv (int n)
{
  struct _opt_var *p;
  int argc;
  for (argc=0,p=opt_first_var;p;p=p->next)
  {
    if (!p->key)
    {
      if (argc++==n)  return (p->value);
    }
  }
  return NULL;
}

/**
  Get the option
  key=field (char *)data in opt_options[]
  return:
    NULL no_key found
    (char *) to saved value
*/
char *opt_get (char *key)
{
  struct _opt_var *p;
  if (key)
  {
    for (p=opt_first_var;p;p=p->next)
    {
      if ((p->key) && (strcmp(p->key,key)==0))
      {
        return (p->value) ? p->value : "1";
      }
    }
  }
  return NULL;
}

/**
  free all memory
  (all but not opt)
*/
void opt_free ()
{
  struct _opt_var *p,*next=NULL;
  for (p=opt_first_var;p;p=next)
  {
    if (p->key)     free (p->key);
    if (p->value)   free (p->value);
    next=p->next;
    free (p);
    
  }
}

/**
  -------------------------------------------------------------------
  This section is refered to config file specific for *nix systems.
  They usually are in $HOME prefix and holding common arguments
  -------------------------------------------------------------------
*/
/**
  option load config file specific *nix machine
  common used arguments are stored in config file.
  comprensible reading allow only long name wit NO_PREFIX
*/
void opt_load_config ()
{
  char *configure_file,*home;
  char *row,*key,*value;
  FILE *sfile;
  int i,counterline=0,index,l=strlen(OPT_CONFIGFILE);
  int (*checkfunc) (int)=NULL;
  
  home=NULL;//getenv ("HOME");
  if (home)
  {
    l=strlen(home);
    configure_file=xmalloc (l+strlen(OPT_CONFIGFILE)+1);
    memcpy (configure_file,home,l);
    memcpy (configure_file+l,OPT_CONFIGFILE,strlen(OPT_CONFIGFILE)+1);
  }
  else
    configure_file=xstrdup (OPT_CONFIGFILE);
  
  if ((sfile=fopen(configure_file,"r"))==NULL)
  {
    fprintf (stderr,"load_config: cannot load '%s' file\n",configure_file);
    free (configure_file);
    return;
  }
  while (!feof(sfile))
  {
    counterline++;
    if ((l=in_string (sfile,&row,0)))
    {
      l=alltrim (row);
      if (*row!=';'&&*row!='#'&&*row!='\n')
      {
        if ((index=opt_search_long (row))>=0)
        {
          if (opt_options[index].type==opt_callback)
          {
            void (*func) (void) = (void (*) (void)) opt_options[index].data;
            func ();
            
            //break;
            //continue;
          }
          key=(char *)opt_options[index].long_name;
          value=row+strlen(opt_options[index].long_name);
          for (;*value && *value==OPT_SPACE;value++);
          if (*value && *value==OPT_ASSIGN[opt_options[index].assign])
            for (value++;*value && *value==OPT_SPACE;value++);    
            
          l=(value)?strlen(value):0;
          if (opt_options[index].assign!=opt_null && l==0)
          {
            fprintf (stderr,"~%s[%d]:\'%s\' "OPT_REQUIRED_ARGUMENT"\n",OPT_CONFIGFILE,counterline,opt_options[index].long_name);
            return;
          }
          else if ((opt_options[index].assign==opt_null || opt_options[index].type==opt_bool) && l)
          {
            fprintf (stderr,"~%s[%d]:\'%s\' "OPT_NOT_REQUIRED_ARGUMENT"\n",OPT_CONFIGFILE,counterline,opt_options[index].long_name);
            return;
          }
          else if (opt_options[index].type==opt_value)
            checkfunc=isdigit;
          else if (opt_options[index].type==opt_string)  
            checkfunc=isprint;     
          if (checkfunc)
          {
            for (i=0;i<l;i++)
            {
              if (checkfunc(value[i])==0)
              {
                fprintf (stderr,"~%s[%d];\'%s\' "OPT_INVALID_PARAMETER" \"",OPT_CONFIGFILE,counterline,opt_options[index].long_name);
                opt_out (stderr,value,l);
                printf ("\"\n");
                return;
              }
            }      
          }
          set_opt (key,value, l);
        }
      }
    }
    free (row);
  }
  fclose (sfile);
  free (configure_file);
}

#if 0

/**
  just the opt_test;
  dumping some values
*/
void opt_test ()
{
  struct _opt_var *p;
  char *dataname;
  int i,argc;
  
  printf ("OPTIONS:\n");
  if (!opt_first_var) printf ("[none]\n");
  for (p=opt_first_var;p;p=p->next)
      printf ("%s=%s\n",p->key,p->value); 
  printf ("\n");
  
  argc=opt_get_argc();
  printf ("THERE %s %d ARGC\n",(argc==1)?"IS":"ARE",argc);
  if (!argc) printf ("[none]\n");
  for (i=0;i<argc;i++)
    printf ("ARGV[%d]=%s\n",i,opt_get_argv(i));
  printf ("\n");

  printf ("CAN EASILY CHECK YOUR ARGUMENTS:\n");
  for (i=0;i<countof(opt_options);i++)
  {
    if (opt_options[i].type!=opt_callback)
    {
      dataname=(char *)opt_options[i].long_name;
      printf ("opt_get(\"%s\")=%s\n",dataname,opt_get(dataname));
    }
  }
}
#endif
#if 0
int main (int argc,char **argv)
{
  char *options_line;
  int err=0;
  
  
  /*
  if (argc<2)
  {
    opt_help ();
    return -1;
  }
  */
  /** need init opt */
  if (opt_init (argc,argv,&options_line)>=0)
  {
    /** load (optional) config file */
    opt_load_config ();
    
    /** the main parser */
    if ((err=opt_index (options_line))==0)
    {
      /** free options_line */
      if (options_line) free (options_line);
    }
  }
  /** lets use it now */
  opt_test ();

  /** remember to free opt enviroment */
  opt_free ();
  
  /** remember to check for errors */
  printf ("\nEXIT with %d\n",err);
  return err;
}
#endif
