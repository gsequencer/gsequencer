/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_MACHINE_COLLECTION_ENTRY_H__
#define __AGS_MACHINE_COLLECTION_ENTRY_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_MACHINE_COLLECTION_ENTRY                (ags_machine_collection_entry_get_type())
#define AGS_MACHINE_COLLECTION_ENTRY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE_COLLECTION_ENTRY, AgsMachineCollectionEntry))
#define AGS_MACHINE_COLLECTION_ENTRY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE_COLLECTION_ENTRY, AgsMachineCollectionEntryClass))
#define AGS_IS_MACHINE_COLLECTION_ENTRY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MACHINE_COLLECTION_ENTRY))
#define AGS_IS_MACHINE_COLLECTION_ENTRY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MACHINE_COLLECTION_ENTRY))
#define AGS_MACHINE_COLLECTION_ENTRY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MACHINE_COLLECTION_ENTRY, AgsMachineCollectionEntryClass))

typedef struct _AgsMachineCollectionEntry AgsMachineCollectionEntry;
typedef struct _AgsMachineCollectionEntryClass AgsMachineCollectionEntryClass;

struct _AgsMachineCollectionEntry
{
  GtkTable table;

  GtkWidget *machine;
};

struct _AgsMachineCollectionEntryClass
{
  GtkTableClass table;
};

GType ags_machine_collection_entry_get_type(void);

AgsMachineCollectionEntry* ags_machine_collection_entry_new();

#endif /*__AGS_MACHINE_COLLECTION_ENTRY_H__*/
