/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AGS_REGISTRY_H__
#define __AGS_REGISTRY_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_uuid.h>

G_BEGIN_DECLS

#define AGS_TYPE_REGISTRY                (ags_registry_get_type())
#define AGS_TYPE_REGISTRY_FLAGS          (ags_registry_flags_get_type())
#define AGS_REGISTRY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_REGISTRY, AgsRegistry))
#define AGS_REGISTRY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_REGISTRY, AgsRegistryClass))
#define AGS_IS_REGISTRY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_REGISTRY))
#define AGS_IS_REGISTRY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_REGISTRY))
#define AGS_REGISTRY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_REGISTRY, AgsRegistryClass))

#define AGS_REGISTRY_GET_OBJ_MUTEX(obj) (&(((AgsRegistry *) obj)->obj_mutex))

typedef struct _AgsRegistry AgsRegistry;
typedef struct _AgsRegistryClass AgsRegistryClass;
typedef struct _AgsRegistryEntry AgsRegistryEntry;

/**
 * AgsRegistryFlags:
 * @AGS_REGISTRY_ADDED_TO_REGISTRY: the registry was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_REGISTRY_CONNECTED: the registry was connected by #AgsConnectable::connect()
 *
 * Enum values to control the behavior or indicate internal state of #AgsRegistry by
 * enable/disable as flags.
 */
typedef enum{
  AGS_REGISTRY_ADDED_TO_REGISTRY   = 1,
  AGS_REGISTRY_CONNECTED           = 1 <<  1,
}AgsRegistryFlags;

struct _AgsRegistry
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;
  
  GObject *server;

  guint counter;

  GList *entry;
};

struct _AgsRegistryClass
{
  GObjectClass gobject;
};

/**
 * AgsRegistryEntry:
 * @id: the #AgsUUID
 * @entry: the actual entry
 * 
 * #AgsRegistryEntry is an entry that you might want to lookup remotely.
 */
struct _AgsRegistryEntry
{
  AgsUUID *id;
  GValue *entry;
};

GType ags_registry_get_type();
GType ags_registry_flags_get_type();

AgsRegistryEntry* ags_registry_entry_alloc();
void ags_registry_entry_free(AgsRegistryEntry *registry_entry);

void ags_registry_add_entry(AgsRegistry *registry,
			    AgsRegistryEntry *registry_entry);

AgsRegistryEntry* ags_registry_find_entry(AgsRegistry *registry,
					  AgsUUID *id);

AgsRegistry* ags_registry_new();

G_END_DECLS

#endif /*__AGS_REGISTRY_H__*/
