/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include "ags_file_dialog.h"

#include <ags/i18n.h>

void ags_file_dialog_class_init(AgsFileDialogClass *file_dialog);
void ags_file_dialog_orientable_interface_init(GtkOrientableIface *orientable);
void ags_file_dialog_init(AgsFileDialog *file_dialog);
void ags_file_dialog_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_file_dialog_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_file_dialog_dispose(GObject *gobject);
void ags_file_dialog_finalize(GObject *gobject);

/**
 * SECTION:ags_file_dialog
 * @short_description: a file dialog widget
 * @title: AgsFileDialog
 * @section_id:
 * @include: ags/widget/ags_file_dialog.h
 *
 * #AgsFileDialog is a dialog widget to open or save files.
 */

enum{
  PROP_0,
};

static gpointer ags_file_dialog_parent_class = NULL;

GType
ags_file_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_file_dialog = 0;

    static const GTypeInfo ags_file_dialog_info = {
      sizeof(AgsFileDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_file_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFileDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_file_dialog_init,
    };

    ags_type_file_dialog = g_type_register_static(GTK_TYPE_WINDOW,
						  "AgsFileDialog", &ags_file_dialog_info,
						  0);    

    g_once_init_leave(&g_define_type_id__volatile, ags_type_file_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_file_dialog_class_init(AgsFileDialogClass *file_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_file_dialog_parent_class = g_type_class_peek_parent(file_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) file_dialog;

  gobject->set_property = ags_file_dialog_set_property;
  gobject->get_property = ags_file_dialog_get_property;

  gobject->dispose = ags_file_dialog_dispose;
  gobject->finalize = ags_file_dialog_finalize;
}

void
ags_file_dialog_init(AgsFileDialog *file_dialog)
{  
  file_dialog->flags = 0;

  file_dialog->vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
					     6);
  gtk_window_set_child((GtkWindow *) file_dialog,
		       (GtkWidget *) file_dialog->vbox);  

  file_dialog->file_widget = ags_file_widget_new();  
  gtk_box_append(file_dialog->vbox,
		 (GtkWidget *) file_dialog->file_widget);
  
  /* button */
  file_dialog->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("open"));
  gtk_box_append(file_dialog->vbox,
		 (GtkWidget *) file_dialog->activate_button);
}

void
ags_file_dialog_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsFileDialog *file_dialog;

  file_dialog = AGS_FILE_DIALOG(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_dialog_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsFileDialog *file_dialog;

  file_dialog = AGS_FILE_DIALOG(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_dialog_dispose(GObject *gobject)
{  
  /* call parent */
  G_OBJECT_CLASS(ags_file_dialog_parent_class)->dispose(gobject);
}

void
ags_file_dialog_finalize(GObject *gobject)
{  
  /* call parent */
  G_OBJECT_CLASS(ags_file_dialog_parent_class)->finalize(gobject);
}

/**
 * ags_file_dialog_new:
 * @transient_for: the #GtkWidget transient-for
 * @title: the window title
 *
 * Creates a new instance of #AgsFileDialog.
 *
 * Returns: the new #AgsFileDialog
 *
 * Since: 6.6.0
 */
AgsFileDialog*
ags_file_dialog_new(GtkWidget *transient_for,
		    gchar *title)
{
  AgsFileDialog *file_dialog;

  file_dialog = (AgsFileDialog *) g_object_new(AGS_TYPE_FILE_DIALOG,
					       "transient-for", transient_for,
					       "title", title,
					       NULL);
  
  return(file_dialog);
}
