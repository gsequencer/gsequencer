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

#ifndef __AGS_SOUND_CONTAINER_H__
#define __AGS_SOUND_CONTAINER_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_SOUND_CONTAINER                    (ags_sound_container_get_type())
#define AGS_SOUND_CONTAINER(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SOUND_CONTAINER, AgsSoundContainer))
#define AGS_SOUND_CONTAINER_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_SOUND_CONTAINER, AgsSoundContainerInterface))
#define AGS_IS_SOUND_CONTAINER(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SOUND_CONTAINER))
#define AGS_IS_SOUND_CONTAINER_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_SOUND_CONTAINER))
#define AGS_SOUND_CONTAINER_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_SOUND_CONTAINER, AgsSoundContainerInterface))

typedef struct _AgsSoundContainer AgsSoundContainer;
typedef struct _AgsSoundContainerInterface AgsSoundContainerInterface;

struct _AgsSoundContainerInterface
{
  GTypeInterface ginterface;
  
  gboolean (*open)(AgsSoundContainer *sound_container, gchar *filename);

  /* level information */
  guint (*get_level_count)(AgsSoundContainer *sound_container);
  guint (*get_nesting_level)(AgsSoundContainer *sound_container);
  
  gchar* (*get_level_id)(AgsSoundContainer *sound_container);
  guint (*get_level_index)(AgsSoundContainer *sound_container);
  
  gchar** (*get_sublevel_name)(AgsSoundContainer *sound_container);
  
  /* levels */
  guint (*level_up)(AgsSoundContainer *sound_container,
		    guint level_count);
  guint (*select_level_by_id)(AgsSoundContainer *sound_container,
			      gchar *level_id);
  guint (*select_level_by_index)(AgsSoundContainer *sound_container,
				 guint level_index);

  /* get sound resource */
  GList* (*get_resource_all)(AgsSoundContainer *sound_container);
  GList* (*get_resource_by_name)(AgsSoundContainer *sound_container,
				 gchar *resource_name);
  GList* (*get_resource_by_index)(AgsSoundContainer *sound_container,
				  guint resource_index);
  GList* (*get_resource_current)(AgsSoundContainer *sound_container);
  
  /* close */
  void (*close)(AgsSoundContainer *sound_resource);
};

GType ags_sound_container_get_type();

gboolean ags_sound_container_open(AgsSoundContainer *sound_container, gchar *filename);

guint ags_sound_container_get_level_count(AgsSoundContainer *sound_container);
guint ags_sound_container_get_nesting_level(AgsSoundContainer *sound_container);
  
gchar* ags_sound_container_get_level_id(AgsSoundContainer *sound_container);
guint ags_sound_container_get_level_index(AgsSoundContainer *sound_container);

gchar** ags_sound_container_get_sublevel_name(AgsSoundContainer *sound_container);

guint ags_sound_container_level_up(AgsSoundContainer *sound_container,
				   guint level_count);
guint ags_sound_container_select_level_by_id(AgsSoundContainer *sound_container,
					     gchar *level_id);
guint ags_sound_container_select_level_by_index(AgsSoundContainer *sound_container,
						guint level_index);

GList* ags_sound_container_get_resource_all(AgsSoundContainer *sound_container);
GList* ags_sound_container_get_resource_by_name(AgsSoundContainer *sound_container,
						gchar *resource_name);
GList* ags_sound_container_get_resource_by_index(AgsSoundContainer *sound_container,
						 guint resource_index);
GList* ags_sound_container_get_resource_current(AgsSoundContainer *sound_container);

void ags_sound_container_close(AgsSoundContainer *sound_container);

#endif /*__AGS_SOUND_CONTAINER_H__*/
