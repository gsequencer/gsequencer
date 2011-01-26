/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/lib/ags_list.h>

void
ags_list_free_and_free_link(GList *list)
{
  GList *list_next;

  while(list != NULL){
    list_next = list->next;
   
    g_free(list->data);
    g_list_free1(list);

    list = list_next;
  }
}

void
ags_list_free_and_unref_link(GList *list)
{
  GList *list_next;

  while(list != NULL){
    list_next = list->next;
   
    g_object_unref((GObject *) list->data);
    g_list_free1(list);

    list = list_next;
  }
}
