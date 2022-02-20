/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2021,2022 Daniel Maksymow, Joël Krähemann
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


#include "ags_meta_data_window.h"

gboolean ags_meta_data_window_delete_event(GtkWidget *widget, GdkEventAny *event);

G_DEFINE_TYPE(AgsMetaDataWindow, ags_meta_data_window, GTK_TYPE_WINDOW)

static void
ags_meta_data_window_class_init(AgsMetaDataWindowClass *klass)
{
  GtkWidgetClass *widget;

  gtk_widget_class_set_template_from_resource(klass,
                                              "/org/nongnu/gsequencer/ags/app/ui/ags_meta_data_window.ui");
  gtk_widget_class_bind_template_child(klass, AgsMetaDataWindow, author);
  gtk_widget_class_bind_template_child(klass, AgsMetaDataWindow, title);
  gtk_widget_class_bind_template_child(klass, AgsMetaDataWindow, album);
  gtk_widget_class_bind_template_child(klass, AgsMetaDataWindow,
                                        release_date);
  gtk_widget_class_bind_template_child(klass, AgsMetaDataWindow,
                                        copyright);
  gtk_widget_class_bind_template_child(klass, AgsMetaDataWindow,
                                        license);
  gtk_widget_class_bind_template_child(klass, AgsMetaDataWindow,
                                        comment);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) klass;

  widget->delete_event = ags_meta_data_window_delete_event;
}

static void
ags_meta_data_window_init(AgsMetaDataWindow *self)
{
  gtk_widget_init_template(self);
}

gboolean
ags_meta_data_window_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  return(TRUE);
}

AgsMetaDataWindow* ags_meta_data_window_new()
{
  return g_object_new(AGS_TYPE_META_DATA_WINDOW, NULL);
}
