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

#ifndef __AGS_SFZ_REGION_H__
#define __AGS_SFZ_REGION_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_SFZ_REGION                (ags_sfz_region_get_type())
#define AGS_SFZ_REGION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SFZ_REGION, AgsSFZRegion))
#define AGS_SFZ_REGION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SFZ_REGION, AgsSFZRegionClass))
#define AGS_IS_SFZ_REGION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SFZ_REGION))
#define AGS_IS_SFZ_REGION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SFZ_REGION))
#define AGS_SFZ_REGION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_SFZ_REGION, AgsSFZRegionClass))

#define AGS_SFZ_REGION_GET_OBJ_MUTEX(obj) (&(((AgsSFZRegion *) obj)->obj_mutex))

typedef struct _AgsSFZRegion AgsSFZRegion;
typedef struct _AgsSFZRegionClass AgsSFZRegionClass;

/**
 * AgsSFZRegionFlags:
 * @AGS_SFZ_REGION_ADDED_TO_REGISTRY: the sfz region was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_SFZ_REGION_CONNECTED: indicates the sfz region was connected by calling #AgsConnectable::connect()
 * 
 * Enum values to control the behavior or indicate internal state of #AgsSFZRegion by
 * enable/disable as flags.
 */
typedef enum{
  AGS_SFZ_REGION_ADDED_TO_REGISTRY    = 1,
  AGS_SFZ_REGION_CONNECTED            = 1 <<  1,
}AgsSFZRegionFlags;

struct _AgsSFZRegion
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;

  AgsUUID *uuid;

  gint nth_region;

  GObject *group;
  GObject *sample;

  GHashTable *control;
};

struct _AgsSFZRegionClass
{
  GObjectClass gobject;
};

GType ags_sfz_region_get_type();

gboolean ags_sfz_region_test_flags(AgsSFZRegion *sfz_region, guint flags);
void ags_sfz_region_set_flags(AgsSFZRegion *sfz_region, guint flags);
void ags_sfz_region_unset_flags(AgsSFZRegion *sfz_region, guint flags);

GObject* ags_sfz_region_get_group(AgsSFZRegion *sfz_region);
void ags_sfz_region_set_group(AgsSFZRegion *sfz_region,
			      GObject *group);

GObject* ags_sfz_region_get_sample(AgsSFZRegion *sfz_region);
void ags_sfz_region_set_sample(AgsSFZRegion *sfz_region,
			       GObject *sample);

GList* ags_sfz_region_get_control(AgsSFZRegion *sfz_region);
void ags_sfz_region_insert_control(AgsSFZRegion *sfz_region,
				   gchar *key,
				   gchar *value);
gchar* ags_sfz_region_lookup_control(AgsSFZRegion *sfz_region,
				     gchar *key);

/* instantiate */
AgsSFZRegion* ags_sfz_region_new();

G_END_DECLS

#endif /*__AGS_SFZ_REGION_H__*/
