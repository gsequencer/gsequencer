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

#include <ags/app/machine/ags_desk_file_chooser.h>

void ags_desk_file_chooser_class_init(AgsDeskFileChooserClass *desk_file_chooser);
void ags_desk_file_chooser_init(AgsDeskFileChooser *desk_file_chooser);
void ags_desk_file_chooser_finalize(GObject *gobject);

/**
 * SECTION:ags_desk_file_chooser
 * @short_description: desk_file_chooser sequencer
 * @title: AgsDeskFileChooser
 * @section_id:
 * @include: ags/app/machine/ags_desk_file_chooser.h
 *
 * The #AgsDeskFileChooser is a composite widget to act as desk_file_chooser sequencer.
 */

static gpointer ags_desk_file_chooser_parent_class = NULL;

GType
ags_desk_file_chooser_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_desk_file_chooser = 0;

    static const GTypeInfo ags_desk_file_chooser_info = {
      sizeof(AgsDeskFileChooserClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_desk_file_chooser_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDeskFileChooser),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_desk_file_chooser_init,
    };

    ags_type_desk_file_chooser = g_type_register_static(GTK_TYPE_GRID,
							"AgsDeskFileChooser", &ags_desk_file_chooser_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_desk_file_chooser);
  }

  return g_define_type_id__volatile;
}

void
ags_desk_file_chooser_class_init(AgsDeskFileChooserClass *desk_file_chooser)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_desk_file_chooser_parent_class = g_type_class_peek_parent(desk_file_chooser);

  /* GObjectClass */
  gobject = (GObjectClass *) desk_file_chooser;

  gobject->finalize = ags_desk_file_chooser_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) desk_file_chooser;
}

void
ags_desk_file_chooser_init(AgsDeskFileChooser *desk_file_chooser)
{
  desk_file_chooser->file_chooser = gtk_file_chooser_widget_new(GTK_FILE_CHOOSER_ACTION_OPEN);

  gtk_widget_set_hexpand((GtkWidget *) desk_file_chooser,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) desk_file_chooser,
			 FALSE);
}

void
ags_desk_file_chooser_finalize(GObject *gobject)
{  
  G_OBJECT_CLASS(ags_desk_file_chooser_parent_class)->finalize(gobject);
}

/**
 * ags_desk_file_chooser_new:
 *
 * Creates an #AgsDeskFileChooser
 *
 * Returns: a new #AgsDeskFileChooser
 *
 * Since: 3.7.0
 */
AgsDeskFileChooser*
ags_desk_file_chooser_new()
{
  AgsDeskFileChooser *desk_file_chooser;

  desk_file_chooser = (AgsDeskFileChooser *) g_object_new(AGS_TYPE_DESK_FILE_CHOOSER,
							  NULL);

  return(desk_file_chooser);
}
