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

#include <ags/X/editor/ags_file_chooser_dialog.h>
#include <ags/X/editor/ags_file_chooser_dialog_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/X/ags_window.h>

void ags_file_chooser_dialog_class_init(AgsFileChooserDialogClass *file_chooser_dialog);
void ags_file_chooser_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_file_chooser_dialog_init(AgsFileChooserDialog *file_chooser_dialog);
void ags_file_chooser_dialog_connect(AgsConnectable *connectable);
void ags_file_chooser_dialog_disconnect(AgsConnectable *connectable);
void ags_file_chooser_dialog_finalize(GObject *gobject);
void ags_file_chooser_dialog_show(GtkWidget *widget);

static gpointer ags_file_chooser_dialog_parent_class = NULL;

GType
ags_file_chooser_dialog_get_type(void)
{
  static GType ags_type_file_chooser_dialog = 0;

  if(!ags_type_file_chooser_dialog){
    static const GTypeInfo ags_file_chooser_dialog_info = {
      sizeof (AgsFileChooserDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_file_chooser_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFileChooserDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_file_chooser_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_file_chooser_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_file_chooser_dialog = g_type_register_static(GTK_TYPE_FILE_CHOOSER_DIALOG,
							  "AgsFileChooserDialog\0", &ags_file_chooser_dialog_info,
							  0);
    
    g_type_add_interface_static(ags_type_file_chooser_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return(ags_type_file_chooser_dialog);
}

void
ags_file_chooser_dialog_class_init(AgsFileChooserDialogClass *file_chooser_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_file_chooser_dialog_parent_class = g_type_class_peek_parent(file_chooser_dialog);

  /* GtkObjectClass */
  gobject = (GObjectClass *) file_chooser_dialog;

  gobject->finalize = ags_file_chooser_dialog_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) file_chooser_dialog;

  widget->show = ags_file_chooser_dialog_show;
}

void
ags_file_chooser_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_file_chooser_dialog_connect;
  connectable->disconnect = ags_file_chooser_dialog_disconnect;
}

void
ags_file_chooser_dialog_init(AgsFileChooserDialog *file_chooser_dialog)
{
  GtkTable *table;
  GtkLabel *label;

  table = (GtkTable *) gtk_table_new(2, 3, FALSE);
  gtk_file_chooser_set_extra_widget(file_chooser_dialog,
				    GTK_WIDGET(table));

  /* first row - preset */
  label = (GtkLabel *) gtk_label_new("preset: \0");
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  file_chooser_dialog->preset = (GtkLabel *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(file_chooser_dialog->preset),
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

  file_chooser_dialog->instrument = (GtkLabel *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(file_chooser_dialog->instrument),
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

  file_chooser_dialog->sample = (GtkLabel *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(file_chooser_dialog->sample),
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_file_chooser_dialog_connect(AgsConnectable *connectable)
{
  AgsFileChooserDialog *file_chooser_dialog;

  /* AgsFileChooserDialog */
  file_chooser_dialog = AGS_FILE_CHOOSER_DIALOG(connectable);

  /* implement me */
}

void
ags_file_chooser_dialog_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_file_chooser_dialog_finalize(GObject *gobject)
{
  AgsFileChooserDialog *file_chooser_dialog;

  file_chooser_dialog = (AgsFileChooserDialog *) gobject;

  /* implement me */

  G_OBJECT_CLASS(ags_file_chooser_dialog_parent_class)->finalize(gobject);
}

void
ags_file_chooser_dialog_show(GtkWidget *widget)
{
  AgsFileChooserDialog *file_chooser_dialog = (AgsFileChooserDialog *) widget;
  AgsWindow *window;
  GtkFrame *frame;

  GTK_WIDGET_CLASS(ags_file_chooser_dialog_parent_class)->show(widget);

  /* perhaps empty */
}

AgsFileChooserDialog*
ags_file_chooser_dialog_new(GObject *devout)
{
  AgsFileChooserDialog *file_chooser_dialog;

  file_chooser_dialog = (AgsFileChooserDialog *) g_object_new(AGS_TYPE_FILE_CHOOSER_DIALOG,
							      NULL);

  return(file_chooser_dialog);
}
