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
#ifndef _CONDITIONS_H
#define _CONDITIONS_H

typedef struct _condition
{
  char *var;
  int var_id;
  
  char *op;
  enum 
  {
    EQ,   // equal to *
    LT,   // less than *
    LE,   // less than or equal to *
    GT,   // greater than *
    GE,   // greater than or equal to *
    NE,   // not equal to *
  }op_id ;
  
  char *val;
  
  struct _condition *next;
  
}CONDITION;

void free_conditions (CONDITION *head);
int make_condition (char *argv, CONDITION **head);
void show_condition (CONDITION *p);

#endif // _CONDITIONS_H