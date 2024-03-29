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

#ifndef __AGS_MACHINE_COLLECTION_H__
#define __AGS_MACHINE_COLLECTION_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/export/ags_machine_mapper.h>

G_BEGIN_DECLS

#define AGS_TYPE_MACHINE_COLLECTION                (ags_machine_collection_get_type())
#define AGS_MACHINE_COLLECTION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE_COLLECTION, AgsMachineCollection))
#define AGS_MACHINE_COLLECTION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE_COLLECTION, AgsMachineCollectionClass))
#define AGS_IS_MACHINE_COLLECTION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MACHINE_COLLECTION))
#define AGS_IS_MACHINE_COLLECTION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MACHINE_COLLECTION))
#define AGS_MACHINE_COLLECTION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MACHINE_COLLECTION, AgsMachineCollectionClass))

typedef struct _AgsMachineCollection AgsMachineCollection;
typedef struct _AgsMachineCollectionClass AgsMachineCollectionClass;

typedef enum{
  AGS_MACHINE_COLLECTION_CONNECTED    = 1,
}AgsMachineCollectionFlags;

struct _AgsMachineCollection
{
  GtkBox box;

  guint flags;
  
  GType machine_mapper_type;

  guint machine_mapper_n_properties;

  gchar **machine_mapper_strv;
  GValue *machine_mapper_value;

  GList *machine_mapper;

  GtkBox *machine_mapper_box;
};

struct _AgsMachineCollectionClass
{
  GtkBoxClass box;
};

GType ags_machine_collection_get_type();

GList* ags_machine_collection_get_machine_mapper(AgsMachineCollection *machine_collection);
void ags_machine_collection_add_machine_mapper(AgsMachineCollection *machine_collection,
					       AgsMachineMapper *machine_mapper);
void ags_machine_collection_remove_machine_mapper(AgsMachineCollection *machine_collection,
						  AgsMachineMapper *machine_mapper);

void ags_machine_collection_reload(AgsMachineCollection *machine_collection);
void ags_machine_collection_add_entry(AgsMachineCollection *machine_collection,
				      GtkWidget *machine);

AgsMachineCollection* ags_machine_collection_new(GType machine_mapper_type,
						 guint machine_mapper_n_properties,
						 gchar **machine_mapper_strv,
						 GValue *machine_mapper_value);

G_END_DECLS

#endif /*__AGS_MACHINE_COLLECTION_H__*/
