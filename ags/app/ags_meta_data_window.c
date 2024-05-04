/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann, Daniel Maksymow
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

#include <ags/app/ags_meta_data_window.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_gsequencer_application.h>

#include <ags/i18n.h>

void ags_meta_data_window_class_init(AgsMetaDataWindowClass *meta_data_window);
void ags_meta_data_window_init(AgsMetaDataWindow *meta_data_window);

gboolean ags_meta_data_window_key_pressed_callback(GtkEventControllerKey *event_controller,
						   guint keyval,
						   guint keycode,
						   GdkModifierType state,
						   AgsMetaDataWindow *meta_data_window);
void ags_meta_data_window_key_released_callback(GtkEventControllerKey *event_controller,
						guint keyval,
						guint keycode,
						GdkModifierType state,
						AgsMetaDataWindow *meta_data_window);
gboolean ags_meta_data_window_modifiers_callback(GtkEventControllerKey *event_controller,
						 GdkModifierType keyval,
						 AgsMetaDataWindow *meta_data_window);

gboolean ags_meta_data_window_close_request_callback(GtkWindow *window, gpointer user_data);

static gpointer ags_meta_data_window_parent_class = NULL;

GType
ags_meta_data_window_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_meta_data_window = 0;

    static const GTypeInfo ags_meta_data_window_info = {
      sizeof (AgsMetaDataWindowClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_meta_data_window_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMetaDataWindow),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_meta_data_window_init,
    };

    ags_type_meta_data_window = g_type_register_static(GTK_TYPE_WINDOW,
						       "AgsMetaDataWindow", &ags_meta_data_window_info,
						       0);
    
    g_once_init_leave(&g_define_type_id__volatile, ags_type_meta_data_window);
  }

  return g_define_type_id__volatile;
}

void
ags_meta_data_window_class_init(AgsMetaDataWindowClass *meta_data_window)
{
  ags_meta_data_window_parent_class = g_type_class_peek_parent(meta_data_window);
}

void
ags_meta_data_window_init(AgsMetaDataWindow *meta_data_window)
{
  GtkGrid *grid;
  GtkScrolledWindow *scrolled_window;
  GtkLabel *label;

  GtkEventController *event_controller;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  ags_ui_provider_set_meta_data_window(AGS_UI_PROVIDER(application_context),
  				       meta_data_window);

  g_object_set(meta_data_window,
	       "title", i18n("meta-data editor"),
	       "hide-on-close", TRUE,
	       NULL);

  gtk_window_set_transient_for((GtkWindow *) meta_data_window,
			       ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context)));

  g_signal_connect(meta_data_window, "close-request",
		   G_CALLBACK(ags_meta_data_window_close_request_callback), NULL);

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) meta_data_window,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_meta_data_window_key_pressed_callback), meta_data_window);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_meta_data_window_key_released_callback), meta_data_window);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_meta_data_window_modifiers_callback), meta_data_window);
  
  grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_window_set_child((GtkWindow *) meta_data_window,
		       (GtkWidget *) grid);

  label = (GtkLabel *) gtk_label_new(i18n("author"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_BASELINE);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  meta_data_window->author = (GtkEntry *) gtk_entry_new();
  gtk_grid_attach(grid,
		  (GtkWidget *) meta_data_window->author,
		  1, 0,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("title"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_BASELINE);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  meta_data_window->title = (GtkEntry *) gtk_entry_new();
  gtk_grid_attach(grid,
		  (GtkWidget *) meta_data_window->title,
		  1, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("album"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_BASELINE);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);

  meta_data_window->album = (GtkEntry *) gtk_entry_new();
  gtk_grid_attach(grid,
		  (GtkWidget *) meta_data_window->album,
		  1, 2,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("release date"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_BASELINE);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);

  meta_data_window->release_date = (GtkEntry *) gtk_entry_new();
  gtk_grid_attach(grid,
		  (GtkWidget *) meta_data_window->release_date,
		  1, 3,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("copyright"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_BASELINE);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);

  meta_data_window->copyright = (GtkEntry *) gtk_entry_new();
  gtk_grid_attach(grid,
		  (GtkWidget *) meta_data_window->copyright,
		  1, 4,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("license"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_BASELINE);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 5,
		  1, 1);

  meta_data_window->license = (GtkEntry *) gtk_entry_new();
  gtk_grid_attach(grid,
		  (GtkWidget *) meta_data_window->license,
		  1, 5,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("comment"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 6,
		  1, 1);

  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();

  gtk_widget_set_size_request((GtkWidget *) scrolled_window,
			      500, 300);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) scrolled_window,
		  1, 6,
		  1, 1);
  
  meta_data_window->comment = (GtkTextView *) gtk_text_view_new();

  gtk_widget_set_halign((GtkWidget *) meta_data_window->comment,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) meta_data_window->comment,
			GTK_ALIGN_FILL);
  
  gtk_scrolled_window_set_child(scrolled_window,
				(GtkWidget *) meta_data_window->comment);
}

gboolean
ags_meta_data_window_key_pressed_callback(GtkEventControllerKey *event_controller,
					  guint keyval,
					  guint keycode,
					  GdkModifierType state,
					  AgsMetaDataWindow *meta_data_window)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }
  
  return(key_handled);
}

void
ags_meta_data_window_key_released_callback(GtkEventControllerKey *event_controller,
					   guint keyval,
					   guint keycode,
					   GdkModifierType state,
					   AgsMetaDataWindow *meta_data_window)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }else{
    switch(keyval){
    case GDK_KEY_Escape:
      {
	gtk_window_close(meta_data_window);	
      }
      break;
    }
  }
}

gboolean
ags_meta_data_window_close_request_callback(GtkWindow *window, gpointer user_data)
{
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  ags_gsequencer_application_refresh_window_menu(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context)));
  
  return(FALSE);
}

gboolean
ags_meta_data_window_modifiers_callback(GtkEventControllerKey *event_controller,
					GdkModifierType keyval,
					AgsMetaDataWindow *meta_data_window)
{
  return(FALSE);
}

/**
 * ags_meta_data_window_new:
 *
 * Creates an #AgsMetaDataWindow
 *
 * Returns: a new #AgsMetaDataWindow
 *
 * Since: 4.0.0
 */
AgsMetaDataWindow*
ags_meta_data_window_new()
{
  AgsMetaDataWindow *meta_data_window;

  meta_data_window = (AgsMetaDataWindow *) g_object_new(AGS_TYPE_META_DATA_WINDOW,
							NULL);

  return(meta_data_window);
}
