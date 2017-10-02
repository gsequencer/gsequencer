/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/widget/ags_container.h>

/**
 * SECTION:ags_container
 * @short_description: Complete GtkContainer
 * @title: AgsContainer
 * @section_id:
 * @include: ags/widget/ags_container.h
 *
 * Functions completing #GtkContainer API.
 */

/**
 * ags_container_remove_all:
 * @container: the #GtkContainer
 * 
 * Adds all #GtkWidget pointed by #GList to @container. 
 *
 * Since: 1.0.0
 */
void
ags_container_add_all(GtkContainer *container,
		      GList *list)
{
  while(list != NULL){
    gtk_container_add(container,
		      GTK_WIDGET(list->data));

    list = list->next;
  }
}
