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

#ifndef __AGS_EFFECT_BULK_ENTRY_H__
#define __AGS_EFFECT_BULK_ENTRY_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_EFFECT_BULK_ENTRY                (ags_effect_bulk_entry_get_type())
#define AGS_EFFECT_BULK_ENTRY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EFFECT_BULK_ENTRY, AgsEffectBulkEntry))
#define AGS_EFFECT_BULK_ENTRY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EFFECT_BULK_ENTRY, AgsEffectBulkEntryClass))
#define AGS_IS_EFFECT_BULK_ENTRY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_EFFECT_BULK_ENTRY))
#define AGS_IS_EFFECT_BULK_ENTRY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_EFFECT_BULK_ENTRY))
#define AGS_EFFECT_BULK_ENTRY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_EFFECT_BULK_ENTRY, AgsEffectBulkEntryClass))

typedef struct _AgsEffectBulkEntry AgsEffectBulkEntry;
typedef struct _AgsEffectBulkEntryClass AgsEffectBulkEntryClass;

struct _AgsEffectBulkEntry
{
  GtkGrid grid;

  GtkCheckButton *check_button;
  GtkLabel *label;
};

struct _AgsEffectBulkEntryClass
{
  GtkGridClass grid;
};

GType ags_effect_bulk_entry_get_type(void);

AgsEffectBulkEntry* ags_effect_bulk_entry_new();

G_END_DECLS

#endif /*__AGS_EFFECT_BULK_ENTRY_H__*/
