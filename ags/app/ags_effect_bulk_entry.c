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

#include <ags/app/ags_effect_bulk_entry.h>

void ags_effect_bulk_entry_class_init(AgsEffectBulkEntryClass *effect_bulk_entry);
void ags_effect_bulk_entry_init(AgsEffectBulkEntry *effect_bulk_entry);
void ags_effect_bulk_entry_finalize(GObject *gobject);

/**
 * SECTION:ags_effect_bulk_entry
 * @short_description: effect bulk entry
 * @title: AgsEffectBulkEntry
 * @section_id:
 * @include: ags/app/ags_effect_bulk_entry.h
 *
 * #AgsEffectBulkEntry is a composite widget to control bulk member.
 */

static gpointer ags_effect_bulk_entry_parent_class = NULL;

GType
ags_effect_bulk_entry_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_effect_bulk_entry = 0;

    static const GTypeInfo ags_effect_bulk_entry_info = {
      sizeof(AgsEffectBulkEntryClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_effect_bulk_entry_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsEffectBulkEntry),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_effect_bulk_entry_init,
    };

    ags_type_effect_bulk_entry = g_type_register_static(GTK_TYPE_GRID,
							"AgsEffectBulkEntry", &ags_effect_bulk_entry_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_effect_bulk_entry);
  }

  return g_define_type_id__volatile;
}

void
ags_effect_bulk_entry_class_init(AgsEffectBulkEntryClass *effect_bulk_entry)
{
  GObjectClass *gobject;

  ags_effect_bulk_entry_parent_class = g_type_class_peek_parent(effect_bulk_entry);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(effect_bulk_entry);

  gobject->finalize = ags_effect_bulk_entry_finalize;
}

void
ags_effect_bulk_entry_init(AgsEffectBulkEntry *effect_bulk_entry)
{
  effect_bulk_entry->check_button = (GtkCheckButton *) gtk_check_button_new();
  gtk_grid_attach(effect_bulk_entry,
		  (GtkWidget *) effect_bulk_entry->check_button,
		  0, 0,
		  1, 1);

  effect_bulk_entry->label = (GtkLabel *) gtk_label_new(NULL);
  gtk_grid_attach(effect_bulk_entry,
		  (GtkWidget *) effect_bulk_entry->label,
		  1, 0,
		  1, 1);
}

void
ags_effect_bulk_entry_finalize(GObject *gobject)
{
  AgsEffectBulkEntry *effect_bulk_entry;

  effect_bulk_entry = (AgsEffectBulkEntry *) gobject;

  /* call parent */
  G_OBJECT_CLASS(ags_effect_bulk_entry_parent_class)->finalize(gobject);
}

/**
 * ags_effect_bulk_entry_new:
 *
 * Creates an #AgsEffectBulkEntry
 *
 * Returns: a new #AgsEffectBulkEntry
 *
 * Since: 3.0.0
 */
AgsEffectBulkEntry*
ags_effect_bulk_entry_new()
{
  AgsEffectBulkEntry *effect_bulk_entry;

  effect_bulk_entry = (AgsEffectBulkEntry *) g_object_new(AGS_TYPE_EFFECT_BULK_ENTRY,
							  NULL);

  return(effect_bulk_entry);
}
