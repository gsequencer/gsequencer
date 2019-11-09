/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_SFZ_GROUP_H__
#define __AGS_SFZ_GROUP_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_SFZ_GROUP                (ags_sfz_group_get_type())
#define AGS_SFZ_GROUP(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SFZ_GROUP, AgsSFZGroup))
#define AGS_SFZ_GROUP_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SFZ_GROUP, AgsSFZGroupClass))
#define AGS_IS_SFZ_GROUP(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SFZ_GROUP))
#define AGS_IS_SFZ_GROUP_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SFZ_GROUP))
#define AGS_SFZ_GROUP_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_SFZ_GROUP, AgsSFZGroupClass))

#define AGS_SFZ_GROUP_GET_OBJ_MUTEX(obj) (&(((AgsSFZGroup *) obj)->obj_mutex))

typedef struct _AgsSFZGroup AgsSFZGroup;
typedef struct _AgsSFZGroupClass AgsSFZGroupClass;

/**
 * AgsSFZGroupFlags:
 * @AGS_SFZ_GROUP_ADDED_TO_REGISTRY: the sfz group was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_SFZ_GROUP_CONNECTED: indicates the sfz group was connected by calling #AgsConnectable::connect()
 * 
 * Enum values to control the behavior or indicate internal state of #AgsSFZGroup by
 * enable/disable as flags.
 */
typedef enum{
  AGS_SFZ_GROUP_ADDED_TO_REGISTRY    = 1,
  AGS_SFZ_GROUP_CONNECTED            = 1 <<  1,
}AgsSFZGroupFlags;

struct _AgsSFZGroup
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;

  AgsUUID *uuid;

  gint nth_group;

  GList *region;
  GObject *sample;
  
  GHashTable *control;
};

struct _AgsSFZGroupClass
{
  GObjectClass gobject;
};

GType ags_sfz_group_get_type();

gboolean ags_sfz_group_test_flags(AgsSFZGroup *sfz_group, guint flags);
void ags_sfz_group_set_flags(AgsSFZGroup *sfz_group, guint flags);
void ags_sfz_group_unset_flags(AgsSFZGroup *sfz_group, guint flags);

void ags_sfz_group_insert_control(AgsSFZGroup *sfz_group,
				  gchar *key,
				  gchar *value);
gchar* ags_sfz_group_lookup_control(AgsSFZGroup *sfz_group,
				    gchar *key);

/* instantiate */
AgsSFZGroup* ags_sfz_group_new();

G_END_DECLS

#endif /*__AGS_SFZ_GROUP_H__*/
