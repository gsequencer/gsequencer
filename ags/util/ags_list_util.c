/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/util/ags_list_util.h>

gpointer ags_list_util_copy(gpointer ptr);
void ags_list_util_free(gpointer ptr);

/**
 * SECTION:ags_list_util
 * @short_description: list util
 * @title: AgsListUtil
 * @section_id:
 * @include: ags/util/ags_list_util.h
 * 
 * List utility.
 */

GType
ags_list_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_list_util = 0;

    ags_type_list_util =
      g_boxed_type_register_static("AgsListUtil",
				   (GBoxedCopyFunc) ags_list_util_copy,
				   (GBoxedFreeFunc) ags_list_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_list_util);
  }

  return g_define_type_id__volatile;
}

gpointer
ags_list_util_copy(gpointer ptr)
{
  gpointer retval;

  retval = g_memdup(ptr, sizeof(AgsListUtil));
 
  return(retval);
}

void
ags_list_util_free(gpointer ptr)
{
  g_free(ptr);
}

/**
 * ags_list_util_find_type:
 * @list: (transfer none) (element-type GObject): the #GList-struct
 * @gtype: the #GType to find
 * 
 * Find @gtype within @list.
 * 
 * Returns: (transfer none) (element-type GObject): the next matching #GList-struct
 * 
 * Since: 3.0.0
 */
GList*
ags_list_util_find_type(GList *list,
			GType gtype)
{
  if(gtype == G_TYPE_NONE){
    return(NULL);
  }
  
  while(list != NULL){
    if(g_type_is_a(G_OBJECT_TYPE(G_OBJECT(list->data)),
		   gtype)){
      return(list);
    }
    
    list = list->next;
  }
  
  return(NULL);
}
