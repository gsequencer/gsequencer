/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/editor/ags_playable_chooser.h>
#include <ags/X/editor/ags_playable_chooser_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/X/ags_window.h>

void ags_playable_chooser_class_init(AgsPlayableChooserClass *playable_chooser);
void ags_playable_chooser_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_playable_chooser_init(AgsPlayableChooser *playable_chooser);
void ags_playable_chooser_connect(AgsConnectable *connectable);
void ags_playable_chooser_disconnect(AgsConnectable *connectable);
void ags_playable_chooser_finalize(GObject *gobject);
void ags_playable_chooser_show(GtkWidget *widget);

static gpointer ags_playable_chooser_parent_class = NULL;

GType
ags_playable_chooser_get_type(void)
{
  static GType ags_type_playable_chooser = 0;

  if(!ags_type_playable_chooser){
    static const GTypeInfo ags_playable_chooser_info = {
      sizeof (AgsPlayableChooserClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_playable_chooser_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayableChooser),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_playable_chooser_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_playable_chooser_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_playable_chooser = g_type_register_static(GTK_TYPE_PLAYABLE_CHOOSER,
						  "AgsPlayableChooser\0", &ags_playable_chooser_info,
						  0);
    
    g_type_add_interface_static(ags_type_playable_chooser,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return(ags_type_playable_chooser);
}

void
ags_playable_chooser_class_init(AgsPlayableChooserClass *playable_chooser)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_playable_chooser_parent_class = g_type_class_peek_parent(playable_chooser);

  /* GtkObjectClass */
  gobject = (GObjectClass *) playable_chooser;

  gobject->finalize = ags_playable_chooser_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) playable_chooser;

  widget->show = ags_playable_chooser_show;
}

void
ags_playable_chooser_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_playable_chooser_connect;
  connectable->disconnect = ags_playable_chooser_disconnect;
}

void
ags_playable_chooser_init(AgsPlayableChooser *playable_chooser)
{
  GtkTable *table;
  GtkLabel *label;

  table = (GtkTable *) gtk_table_new(2, 3, FALSE);
  gtk_file_chooser_set_extra_widget(playable_chooser,
				    GTK_WIDGET(table));

  /* first row - preset */
  label = (GtkLabel *) gtk_label_new("preset: \0");
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  playable_chooser->preset = (GtkLabel *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(playable_chooser->preset),
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* second row - instrument */
  label = (GtkLabel *) gtk_label_new("instrument: \0");
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  playable_chooser->instrument = (GtkLabel *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(playable_chooser->instrument),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* third row - sample */
  label = (GtkLabel *) gtk_label_new("sample: \0");
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  playable_chooser->sample = (GtkLabel *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(playable_chooser->sample),
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_playable_chooser_connect(AgsConnectable *connectable)
{
  AgsPlayableChooser *playable_chooser;

  /* AgsPlayableChooser */
  playable_chooser = AGS_PLAYABLE_CHOOSER(connectable);

  /* implement me */
}

void
ags_playable_chooser_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_playable_chooser_finalize(GObject *gobject)
{
  AgsPlayableChooser *playable_chooser;

  playable_chooser = (AgsPlayableChooser *) gobject;

  /* implement me */

  G_OBJECT_CLASS(ags_playable_chooser_parent_class)->finalize(gobject);
}

void
ags_playable_chooser_show(GtkWidget *widget)
{
  AgsPlayableChooser *playable_chooser = (AgsPlayableChooser *) widget;

  GTK_WIDGET_CLASS(ags_playable_chooser_parent_class)->show(widget);

  /* perhaps empty */
}

AgsPlayableChooser*
ags_playable_chooser_new(GObject *devout)
{
  AgsPlayableChooser *playable_chooser;

  playable_chooser = (AgsPlayableChooser *) g_object_new(AGS_TYPE_PLAYABLE_CHOOSER,
							 NULL);
  
  return(playable_chooser);
}
