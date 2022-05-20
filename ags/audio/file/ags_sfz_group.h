/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

struct _AgsSFZGroup
{
  GObject gobject;

  guint flags;
  guint connectable_flags;

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

GList* ags_sfz_group_get_region(AgsSFZGroup *sfz_group);
void ags_sfz_group_set_region(AgsSFZGroup *sfz_group,
			     GList *region);

GObject* ags_sfz_group_get_sample(AgsSFZGroup *sfz_group);
void ags_sfz_group_set_sample(AgsSFZGroup *sfz_group,
			      GObject *sample);

GList* ags_sfz_group_get_control(AgsSFZGroup *sfz_group);
void ags_sfz_group_insert_control(AgsSFZGroup *sfz_group,
				  gchar *key,
				  gchar *value);
gchar* ags_sfz_group_lookup_control(AgsSFZGroup *sfz_group,
				    gchar *key);

/* instantiate */
AgsSFZGroup* ags_sfz_group_new();

G_END_DECLS

#endif /*__AGS_SFZ_GROUP_H__*/
