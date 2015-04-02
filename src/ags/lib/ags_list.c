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

/**
 * SECTION:ags_list
 * @short_description: Complete GList
 * @title: AgsList
 * @section_id:
 * @include: ags/lib/ags_list.h
 *
 * Functions completing #GList API.
 */

/**
 * ags_list_free_and_free_link:
 * @list: the #GList
 * 
 * Free list and free data.
 *
 * Since: 0.3
 * Deprecated: Use g_list_free_full() with g_free() instead
 */
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


/**
 * ags_list_free_and_unref_link:
 * @list: the #GList
 * 
 * Free list and unref data.
 *
 * Since: 0.3
 * Deprecated: Use g_list_free_full() with g_ojbect_unref() instead
 */
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

/**
 * ags_list_duplicat_list:
 * @entry: the entry to prepend
 * @target: location to store the #GList 
 *
 * ???
 *
 * Since: 0.3
 * Deprecated: Ever used?
 */
void
ags_list_duplicate_list(gpointer entry, gpointer target)
{
  GList **list;

  list = (GList **) target;

  *list = g_list_prepend(*list,
			 entry);
}
