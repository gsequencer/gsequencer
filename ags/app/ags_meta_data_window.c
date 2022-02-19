/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2021 Daniel Maksymow
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

struct _AgsMetaDataWindow
{
  GtkWindow parent;
  GtkEntry *author_entry;
  GtkEntry *title_entry;
  GtkEntry *album_entry;
  GtkEntry *release_date_entry;
  GtkEntry *copyright_entry;
  GtkEntry *license_entry;
  GtkEntry *comment_entry;
};

G_DEFINE_TYPE(AgsMetaDataWindow, ags_meta_data_window, GTK_TYPE_WINDOW)

static void
on_update_clicked_cb(GtkButton *button, AgsMetaDataWindow *window)
{
  g_message("clicked: %s", gtk_entry_get_text(window->author_entry));
}

static void
ags_meta_data_window_class_init(AgsMetaDataWindowClass *klass)
{
  gtk_widget_class_set_template_from_resource(klass,
                                              "/ags/app/ui/ags_meta_data_window.ui");
  gtk_widget_class_bind_template_child(klass, AgsMetaDataWindow, author_entry);
  gtk_widget_class_bind_template_child(klass, AgsMetaDataWindow, title_entry);
  gtk_widget_class_bind_template_child(klass, AgsMetaDataWindow, album_entry);
  gtk_widget_class_bind_template_child(klass, AgsMetaDataWindow,
                                        release_date_entry);
  gtk_widget_class_bind_template_child(klass, AgsMetaDataWindow,
                                        copyright_entry);
  gtk_widget_class_bind_template_child(klass, AgsMetaDataWindow,
                                        license_entry);
  gtk_widget_class_bind_template_child(klass, AgsMetaDataWindow,
                                        comment_entry);
  gtk_widget_class_bind_template_callback(klass, on_update_clicked_cb);
}

static void
ags_meta_data_window_init(AgsMetaDataWindow *self)
{
  gtk_widget_init_template(self);
}

AgsMetaDataWindow* ags_meta_data_window_new()
{
  return g_object_new(AGS_TYPE_META_DATA_WINDOW, NULL);
}
