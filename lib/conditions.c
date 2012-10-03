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
#include <ctype.h>

#include "mem.h"
#include "util.h"
#include "conditions.h"


int check_op (char *op, char *var)
{
  static char *_op[]={"=","<","<=",">",">=","!="};
  int i,n=sizeof(_op)/sizeof(_op[0]);
  
  for (i=0;i<n;i++)
    if (!strcmp(_op[i],op))
      return i;
  
  printf ("error: ‘%s’ invalid operator for the argument ‘%s’\n",op,var);    
  
  return -1;
}

int make_condition (char *argv, CONDITION **head)
{
  const char *delim="<>=!";
  
  int i,j;
  int op_id;
  CONDITION *new_condition;
  char *var;
  char *op;
  char *val;
  
  *head=NULL;
  
  j=0;
  while (argv[j])
  {
    for (i=j;argv[j] && isspace(argv[j]);j++);
    for (i=j;argv[j] && !isspace(argv[j]) && !strchr(delim,argv[j]);j++);
    if (i==j)
    {
      printf ("Error: undefined argument\n");
      return -1;
    }
    var=mem2str(argv+i, j-i);
    if ((xstrcmpi(var,"AND"))==0)
    {
      free (var);
      continue;
    }
    
    for (i=j;argv[j] && isspace(argv[j]);j++);
    for (i=j;argv[j] && strchr(delim,argv[j]) ;j++);
    if (i==j)
    {
      printf ("error: undefined operator for the argument ‘%s’\n",var);    
      free (var);
      return -1;
    }
    op=mem2str(argv+i, j-i);
    if ((op_id=check_op (op,var))<0)
    {
      free (var);
      free (op);
      return -1;
    }
    
    for (i=j;argv[j] && isspace(argv[j]);j++);
    if (argv[j]=='\'')
    {
      for (i=++j;argv[j] && argv[j]!='\'';j++);
      if (!argv[j])
      {
        printf ("Error: unclosed single quote\n");
        free (var);
        free (op);
        return -1;
      }
    }
    else 
      for (i=j;argv[j] && !isspace(argv[j]);j++);
      
    if (i==j)
    {
      printf ("error: undefined value for the argument ‘%s %s’\n",var,op);    
      free (var);
      free (op);
      return -1;
    }
    val=mem2str(argv+i, j-i);
    if (argv[j]=='\'') 
      j++;
    
    for (i=j;argv[j] && isspace(argv[j]);j++);
    
    if ((new_condition=xmalloc (sizeof(CONDITION)))==NULL)
      return -1;
    
    new_condition->var=var;
    new_condition->var_id=0;
    new_condition->op=op;
    new_condition->op_id=op_id;
    new_condition->val=val;
    new_condition->next=*head;
    
    *head=new_condition;
   
  }
  return 0;
}

void free_conditions (CONDITION *head)
{
  CONDITION *next;
  while (head)
  {
    free (head->val);
    free (head->op);
    free (head->var);
    
    next=head->next;
    free (head);
    head=next;
  }
}

void show_condition (CONDITION *p)
{
  for (;p;p=p->next)
  {
    printf ("var=\'%s\'\n",p->var);
    printf ("var_id=\'%d\'\n",p->var_id);
    printf ("op=\'%s\'\n",p->op);
    printf ("op_id=\'%d\'\n",p->op_id);
    printf ("val=\'%s\'\n",p->val);   
  }
}
/*
CONDITION *_dbf_load_conditions (DBF *dbf)
{
}
*/
#if 0
int main (int argc, char **argv)
{
  CONDITION *head,*p;
  make_condition (argv[1],&head);
  
  p=head;
  for (p=head;p;p=p->next)
  {
    printf ("var=\'%s\'\n",p->var);
    printf ("var_id=\'%d\'\n",p->var_id);
    printf ("op=\'%s\'\n",p->op);
    printf ("op_id=\'%d\'\n",p->op_id);
    printf ("val=\'%s\'\n",p->val);   

  }
  free_conditions (head);
  return 0;
}
#endif