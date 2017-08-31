/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <pthread.h>

#include <glib.h>
#include <glib-object.h>

#ifdef AGS_WITH_XMLRPC_C
#include <xmlrpc.h>
#include <xmlrpc_server.h>
#endif

#include <ags/file/ags_file.h>

#define AGS_TYPE_REGISTRY                (ags_registry_get_type())
#define AGS_REGISTRY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_REGISTRY, AgsRegistry))
#define AGS_REGISTRY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_REGISTRY, AgsRegistryClass))
#define AGS_IS_REGISTRY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_REGISTRY))
#define AGS_IS_REGISTRY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_REGISTRY))
#define AGS_REGISTRY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_REGISTRY, AgsRegistryClass))

#define AGS_REGISTRY_DEFAULT_ID_LENGTH (36)

typedef struct _AgsRegistry AgsRegistry;
typedef struct _AgsRegistryClass AgsRegistryClass;
typedef struct _AgsRegistryEntry AgsRegistryEntry;

typedef enum{
  AGS_REGISTRY_CONNECTED     = 1,
  AGS_REGISTRY_XML_DOCUMENT  = 1 <<  1,
  AGS_REGISTRY_XML_RPC       = 1 <<  2,
  AGS_REGISTRY_DIRTY         = 1 <<  3,
}AgsRegistryFlags;

struct _AgsRegistry
{
  GObject object;

  guint flags;

  pthread_mutex_t *mutex;
  pthread_mutexattr_t *mutexattr;

  AgsFile *previous;
  AgsFile *current;

#ifdef AGS_WITH_XMLRPC_C
  xmlrpc_registry *registry;
#else
  gpointer registry;
#endif
  
  GObject *server;

  guint id_length;
  guint counter;

  GList *entry;
};

struct _AgsRegistryClass
{
  GObjectClass object;
};

struct _AgsRegistryEntry
{
  gchar *id;
  GValue entry;
};

GType ags_registry_get_type();

AgsRegistryEntry* ags_registry_entry_alloc(AgsRegistry *registry);
void ags_registry_entry_free(AgsRegistryEntry *registry_entry);

void ags_registry_add(AgsRegistry *registry,
		      AgsRegistryEntry *registry_entry);

AgsRegistryEntry* ags_registry_entry_find(AgsRegistry *registry,
					  gchar *id);

AgsRegistry* ags_registry_new();

#endif /*__AGS_REGISTRY_H__*/
