/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/file/ags_sound_container.h>

#include <ags/libags.h>

void ags_sound_container_base_init(AgsSoundContainerInterface *interface);

/**
 * SECTION:ags_sound_container
 * @short_description: read/write audio
 * @title: AgsSoundContainer
 * @section_id:
 * @include: ags/audio/file/ags_sound_container.h
 *
 * The #AgsSoundContainer interface gives you a unique access to file related
 * IO operations.
 */

GType
ags_sound_container_get_type()
{
  static GType ags_type_sound_container = 0;

  if(!ags_type_sound_container){
    static const GTypeInfo ags_sound_container_info = {
      sizeof(AgsSoundContainerInterface),
      (GBaseInitFunc) ags_sound_container_base_init,
      NULL, /* base_finalize */
    };

    ags_type_sound_container = g_type_register_static(G_TYPE_INTERFACE,
						      "AgsSoundContainer", &ags_sound_container_info,
						      0);
  }

  return(ags_type_sound_container);
}


void
ags_sound_container_base_init(AgsSoundContainerInterface *interface)
{
  /* empty */
}

guint
ags_sound_container_get_level_count(AgsSoundContainer *sound_container)
{
  AgsSoundContainerInterface *sound_container_interface;

  guint retval;

  g_return_val_if_fail(AGS_IS_SOUND_CONTAINER(sound_container), 0);
  sound_container_interface = AGS_SOUND_CONTAINER_GET_INTERFACE(sound_container);
  g_return_val_if_fail(sound_container_interface->get_level_count, 0);

  retval = sound_container_interface->get_level_count(sound_container);

  return(retval);
}

guint
ags_sound_container_get_nesting_level(AgsSoundContainer *sound_container)
{
  AgsSoundContainerInterface *sound_container_interface;

  guint retval;

  g_return_val_if_fail(AGS_IS_SOUND_CONTAINER(sound_container), 0);
  sound_container_interface = AGS_SOUND_CONTAINER_GET_INTERFACE(sound_container);
  g_return_val_if_fail(sound_container_interface->get_nesting_level, 0);

  retval = sound_container_interface->get_nesting_level(sound_container);

  return(retval);
}

gchar*
ags_sound_container_get_level_id(AgsSoundContainer *sound_container)
{
  AgsSoundContainerInterface *sound_container_interface;

  gchar *retval;

  g_return_val_if_fail(AGS_IS_SOUND_CONTAINER(sound_container), NULL);
  sound_container_interface = AGS_SOUND_CONTAINER_GET_INTERFACE(sound_container);
  g_return_val_if_fail(sound_container_interface->get_level_id, NULL);

  retval = sound_container_interface->get_level_id(sound_container);

  return(retval);
}

guint
ags_sound_container_get_level_index(AgsSoundContainer *sound_container)
{
  AgsSoundContainerInterface *sound_container_interface;

  guint retval;

  g_return_val_if_fail(AGS_IS_SOUND_CONTAINER(sound_container), 0);
  sound_container_interface = AGS_SOUND_CONTAINER_GET_INTERFACE(sound_container);
  g_return_val_if_fail(sound_container_interface->get_level_index, 0);

  retval = sound_container_interface->get_level_index(sound_container);

  return(retval);
}

guint
ags_sound_container_level_up(AgsSoundContainer *sound_container,
			     guint level_count)
{
  AgsSoundContainerInterface *sound_container_interface;

  guint retval;

  g_return_val_if_fail(AGS_IS_SOUND_CONTAINER(sound_container), 0);
  sound_container_interface = AGS_SOUND_CONTAINER_GET_INTERFACE(sound_container);
  g_return_val_if_fail(sound_container_interface->level_up, 0);

  retval = sound_container_interface->level_up(sound_container,
					       level_count);

  return(retval);
}

guint
ags_sound_container_select_level_by_id(AgsSoundContainer *sound_container,
				       gchar *level_id)
{
  AgsSoundContainerInterface *sound_container_interface;

  guint retval;

  g_return_val_if_fail(AGS_IS_SOUND_CONTAINER(sound_container), 0);
  sound_container_interface = AGS_SOUND_CONTAINER_GET_INTERFACE(sound_container);
  g_return_val_if_fail(sound_container_interface->select_level_by_id, 0);

  retval = sound_container_interface->select_level_by_id(sound_container);

  return(retval);
}

guint
ags_sound_container_select_level_by_index(AgsSoundContainer *sound_container,
					  guint level_index)
{
  AgsSoundContainerInterface *sound_container_interface;

  guint retval;

  g_return_val_if_fail(AGS_IS_SOUND_CONTAINER(sound_container), 0);
  sound_container_interface = AGS_SOUND_CONTAINER_GET_INTERFACE(sound_container);
  g_return_val_if_fail(sound_container_interface->select_level_by_index, 0);

  retval = sound_container_interface->select_level_by_index(sound_container);

  return(retval);
}

GList*
ags_sound_container_get_resource_all(AgsSoundContainer *sound_container)
{
  AgsSoundContainerInterface *sound_container_interface;

  GList *retval;

  g_return_val_if_fail(AGS_IS_SOUND_CONTAINER(sound_container), NULL);
  sound_container_interface = AGS_SOUND_CONTAINER_GET_INTERFACE(sound_container);
  g_return_val_if_fail(sound_container_interface->get_resource_all, NULL);

  retval = sound_container_interface->get_resource_all(sound_container);

  return(retval);
}

GList*
ags_sound_container_get_resource_by_name(AgsSoundContainer *sound_container,
					 gchar *resource_name)
{
  AgsSoundContainerInterface *sound_container_interface;

  GList *retval;

  g_return_val_if_fail(AGS_IS_SOUND_CONTAINER(sound_container), NULL);
  sound_container_interface = AGS_SOUND_CONTAINER_GET_INTERFACE(sound_container);
  g_return_val_if_fail(sound_container_interface->get_resource_by_name, NULL);

  retval = sound_container_interface->get_resource_by_name(sound_container,
							   resource_name);

  return(retval);
}

GList*
ags_sound_container_get_resource_by_index(AgsSoundContainer *sound_container,
					  guint resource_index)
{
  AgsSoundContainerInterface *sound_container_interface;

  GList *retval;

  g_return_val_if_fail(AGS_IS_SOUND_CONTAINER(sound_container), NULL);
  sound_container_interface = AGS_SOUND_CONTAINER_GET_INTERFACE(sound_container);
  g_return_val_if_fail(sound_container_interface->get_resource_by_index, NULL);

  retval = sound_container_interface->get_resource_by_index(sound_container,
							    resource_index);

  return(retval);
}
