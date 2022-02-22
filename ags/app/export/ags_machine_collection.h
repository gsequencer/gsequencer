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

#ifndef __AGS_MACHINE_COLLECTION_H__
#define __AGS_MACHINE_COLLECTION_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

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
  
  GType child_type;

  guint child_n_properties;

  gchar **child_strv;
  GValue *child_value;

  GtkBox *child;
};

struct _AgsMachineCollectionClass
{
  GtkBoxClass box;
};

GType ags_machine_collection_get_type();

void ags_machine_collection_reload(AgsMachineCollection *machine_collection);
void ags_machine_collection_add_entry(AgsMachineCollection *machine_collection,
				      GtkWidget *machine);

AgsMachineCollection* ags_machine_collection_new(GType child_type,
						 guint child_n_properties,
						 gchar **child_strv,
						 GValue *child_value);

G_END_DECLS

#endif /*__AGS_MACHINE_COLLECTION_H__*/
