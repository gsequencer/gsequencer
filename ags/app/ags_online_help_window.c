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

#include <ags/app/ags_online_help_window.h>
#include <ags/app/ags_online_help_window_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <stdlib.h>
#include <string.h>

#include <ags/i18n.h>

void ags_online_help_window_class_init(AgsOnlineHelpWindowClass *online_help_window);
void ags_online_help_window_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_online_help_window_init(AgsOnlineHelpWindow *online_help_window);

void ags_online_help_window_connect(AgsConnectable *connectable);
void ags_online_help_window_disconnect(AgsConnectable *connectable);

static gpointer ags_online_help_window_parent_class = NULL;

GType
ags_online_help_window_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_online_help_window = 0;

    static const GTypeInfo ags_online_help_window_info = {
      sizeof (AgsOnlineHelpWindowClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_online_help_window_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOnlineHelpWindow),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_online_help_window_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_online_help_window_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_online_help_window = g_type_register_static(GTK_TYPE_WINDOW,
							 "AgsOnlineHelpWindow", &ags_online_help_window_info,
							 0);
    
    g_type_add_interface_static(ags_type_online_help_window,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_online_help_window);
  }

  return g_define_type_id__volatile;
}

void
ags_online_help_window_class_init(AgsOnlineHelpWindowClass *online_help_window)
{  
  ags_online_help_window_parent_class = g_type_class_peek_parent(online_help_window);
}

void
ags_online_help_window_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;

  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = NULL;
  connectable->connect = ags_online_help_window_connect;
  connectable->disconnect = ags_online_help_window_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_online_help_window_init(AgsOnlineHelpWindow *online_help_window)
{

#if defined(AGS_WITH_POPPLER)
  GtkBox *vbox;
  GtkGrid *grid;
  GtkLabel *label;
  
  GtkAdjustment *vadjustment, *hadjustment;
  GtkAllocation allocation;

  cairo_t *cr;

  gchar *data;
  gchar *pdf_filename;

  gsize length;
  gint num_pages, i;
  gdouble width, height;
  gint max_width, max_height;
  
  GError *error;

  g_object_set(online_help_window,
	       "title", i18n("Online help"),
	       NULL);

  gtk_window_set_hide_on_close((GtkWindow *) online_help_window,
			       TRUE);

  online_help_window->flags = AGS_ONLINE_HELP_WINDOW_SHOW_PDF_CONTROLS;
  online_help_window->connectable_flags = 0;
  
  width = 800.0;
  height = 600.0;
  
  g_object_set(online_help_window,
	       "default-width", (gint) width,
	       "default-height", (gint) height,
	       NULL);

  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_window_set_child(online_help_window,
		       (GtkWidget *) vbox);
  
  online_help_window->pdf_controls = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
							    AGS_UI_PROVIDER_DEFAULT_SPACING);

  //  gtk_widget_set_visible(online_help_window->pdf_controls,
  //			 TRUE);

  gtk_box_append(vbox,
		 (GtkWidget *) online_help_window->pdf_controls);

  label = (GtkLabel *) gtk_label_new(i18n("zoom"));  
  gtk_box_append(online_help_window->pdf_controls,
		 (GtkWidget *) label);

  online_help_window->zoom = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text((GtkComboBoxText *) online_help_window->zoom,
				 "50 %");
  gtk_combo_box_text_append_text((GtkComboBoxText *) online_help_window->zoom,
				 "75 %");
  gtk_combo_box_text_append_text((GtkComboBoxText *) online_help_window->zoom,
				 "100 %");
  gtk_combo_box_text_append_text((GtkComboBoxText *) online_help_window->zoom,
				 "125 %");
  gtk_combo_box_text_append_text((GtkComboBoxText *) online_help_window->zoom,
				 "150 %");
  gtk_combo_box_text_append_text((GtkComboBoxText *) online_help_window->zoom,
				 "200 %");

  gtk_combo_box_set_active(online_help_window->zoom,
			   2);
  
  gtk_box_append(online_help_window->pdf_controls,
		 (GtkWidget *) online_help_window->zoom);
  
  grid = gtk_grid_new();

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append(vbox,
		 (GtkWidget *) grid);
  
  online_help_window->pdf_drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_vexpand(online_help_window->pdf_drawing_area,
			 TRUE);
  gtk_widget_set_hexpand(online_help_window->pdf_drawing_area,
			 TRUE);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) online_help_window->pdf_drawing_area,
		  0, 0,
		  1, 1);

  vadjustment = gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.2, 1.0);

  online_help_window->pdf_vscrollbar = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL,
							 vadjustment);
  gtk_grid_attach(grid,
		  (GtkWidget *) online_help_window->pdf_vscrollbar,
		  1, 0,
		  1, 1);
  
  hadjustment = gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.2, 1.0);
  
  online_help_window->pdf_hscrollbar = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL,
							 hadjustment);
  gtk_grid_attach(grid,
		  (GtkWidget *) online_help_window->pdf_hscrollbar,
		  0, 1,
		  1, 1);

#ifdef AGS_ONLINE_HELP_PDF_FILENAME
  pdf_filename = g_strdup(AGS_ONLINE_HELP_PDF_FILENAME);
#else
  if((pdf_filename = getenv("AGS_ONLINE_HELP_PDF_FILENAME")) != NULL){
    pdf_filename = g_strdup(pdf_filename);
  }else{
#if defined (AGS_W32API)
    AgsApplicationContext *application_context;
    
    gchar *app_dir;
    
    application_context = ags_application_context_get_instance();

    app_dir = NULL;

    if(strlen(application_context->argv[0]) > strlen("\\gsequencer.exe")){
      app_dir = g_strndup(application_context->argv[0],
			  strlen(application_context->argv[0]) - strlen("\\gsequencer.exe"));
    }
  
    pdf_filename = g_strdup_printf("%s\\share\\doc\\gsequencer-doc\\pdf\\ags-user-manual.pdf",
				   g_get_current_dir());
    
    if(!g_file_test(pdf_filename,
		    G_FILE_TEST_IS_REGULAR)){
      g_free(pdf_filename);

      if(g_path_is_absolute(app_dir)){
	pdf_filename = g_strdup_printf("%s\\%s",
				       app_dir,
				       "\\share\\doc\\gsequencer-doc\\pdf\\ags-user-manual.pdf");
      }else{
	pdf_filename = g_strdup_printf("%s\\%s\\%s",
				       g_get_current_dir(),
				       app_dir,
				       "\\share\\doc\\gsequencer-doc\\pdf\\ags-user-manual.pdf");
      }
    }

    g_free(app_dir);
#else  
    pdf_filename = g_strdup_printf("%s%s", AGS_DOC_DIR, "/pdf/user-manual.pdf");
#endif
  }
#endif

  error = NULL;
  g_file_get_contents(pdf_filename,
		      &data,
		      &length,
		      &error);
  
  error = NULL;
  online_help_window->pdf_document = poppler_document_new_from_data(data,
								    length,
								    NULL,
								    &error);

  num_pages = poppler_document_get_n_pages(online_help_window->pdf_document);

  max_width = 0;
  max_height = 0;

  for(i = 0; i < num_pages; i++){
    PopplerPage *page;
    
    page = poppler_document_get_page(online_help_window->pdf_document,
				     i);
    
    if(page == NULL) {
      g_warning("poppler fail: page not found");

      break;
    }

    poppler_page_get_size(page,
			  &width, &height);

    if(max_width < width){
      max_width = width;
    }

    max_height += height;
    
    g_object_unref(page);
  }

  online_help_window->zoom_x = 1.0;
  online_help_window->zoom_y = 1.0;

  online_help_window->max_height = max_height;
  online_help_window->max_width = max_width;
  
  gtk_widget_get_allocation(online_help_window->pdf_drawing_area,
			    &allocation);

  gtk_adjustment_configure(vadjustment,
			   0.0,
			   0.0,
			   (double) online_help_window->max_height - (double) allocation.height,
			   5.0,
			   15.0,
			   (double) 10.0);

  gtk_adjustment_configure(hadjustment,
			   0.0,
			   0.0,
			   (double) online_help_window->max_width - (double) allocation.width,
			   5.0,
			   15.0,
			   (double) 100.0);
#endif
}

void
ags_online_help_window_connect(AgsConnectable *connectable)
{
  AgsOnlineHelpWindow *online_help_window;

  online_help_window = AGS_ONLINE_HELP_WINDOW(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (online_help_window->connectable_flags)) != 0){
    return;
  }

  online_help_window->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

#if defined(AGS_WITH_POPPLER)
  gtk_drawing_area_set_draw_func(online_help_window->pdf_drawing_area,
				 ags_online_help_window_pdf_drawing_area_draw_callback,
				 online_help_window,
				 NULL);

  g_signal_connect_after(G_OBJECT(online_help_window->zoom), "changed",
			 G_CALLBACK(ags_online_help_window_pdf_zoom_changed_callback), online_help_window);

  g_signal_connect_after(G_OBJECT(online_help_window->pdf_drawing_area), "resize",
			 G_CALLBACK(ags_online_help_window_pdf_drawing_area_resize_callback), online_help_window);

  g_signal_connect_after(G_OBJECT(gtk_scrollbar_get_adjustment(online_help_window->pdf_vscrollbar)), "value-changed",
			 G_CALLBACK(ags_online_help_window_pdf_vscrollbar_value_changed_callback), online_help_window);

  g_signal_connect_after(G_OBJECT(gtk_scrollbar_get_adjustment(online_help_window->pdf_hscrollbar)), "value-changed",
			 G_CALLBACK(ags_online_help_window_pdf_hscrollbar_value_changed_callback), online_help_window);
#endif  
}

void
ags_online_help_window_disconnect(AgsConnectable *connectable)
{
  AgsOnlineHelpWindow *online_help_window;

  online_help_window = AGS_ONLINE_HELP_WINDOW(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (online_help_window->connectable_flags)) == 0){
    return;
  }

  online_help_window->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

#if defined(AGS_WITH_POPPLER)
  gtk_drawing_area_set_draw_func(online_help_window->pdf_drawing_area,
				 NULL,
				 online_help_window,
				 NULL);

  g_object_disconnect(G_OBJECT(online_help_window->zoom),
		      "any_signal::changed",
		      G_CALLBACK(ags_online_help_window_pdf_zoom_changed_callback),
		      online_help_window,
		      NULL);

  g_object_disconnect(G_OBJECT(online_help_window->pdf_drawing_area),
		      "any_signal::resize",
		      G_CALLBACK(ags_online_help_window_pdf_drawing_area_resize_callback),
		      online_help_window,
		      NULL);

  g_object_disconnect(G_OBJECT(online_help_window->pdf_vscrollbar),
		      "value-changed",
		      G_CALLBACK(ags_online_help_window_pdf_vscrollbar_value_changed_callback),
		      online_help_window,
		      NULL);

  g_object_disconnect(G_OBJECT(online_help_window->pdf_hscrollbar),
		      "value-changed",
		      G_CALLBACK(ags_online_help_window_pdf_hscrollbar_value_changed_callback),
		      online_help_window,
		      NULL);
#endif  
}

/**
 * ags_online_help_window_new:
 * @transient_for: the #GtkWindow
 *
 * Creates an #AgsOnlineHelpWindow
 *
 * Returns: a new #AgsOnlineHelpWindow
 *
 * Since: 3.5.0
 */
AgsOnlineHelpWindow*
ags_online_help_window_new(GtkWindow *transient_for)
{
  AgsOnlineHelpWindow *online_help_window;

  online_help_window = (AgsOnlineHelpWindow *) g_object_new(AGS_TYPE_ONLINE_HELP_WINDOW,
							    "transient-for", transient_for,
							    NULL);

  return(online_help_window);
}
