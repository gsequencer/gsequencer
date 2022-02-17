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

#include <ags/GSequencer/ags_animation_window.h>

#include <ags/GSequencer/ags_ui_provider.h>
#include <ags/GSequencer/ags_window.h>

#include <stdlib.h>
#include <string.h>

#include <ags/i18n.h>

void ags_animation_window_class_init(AgsAnimationWindowClass *animation_window);
void ags_animation_window_init(AgsAnimationWindow *animation_window);
void ags_animation_window_show(GtkWidget *widget);

gboolean ags_animation_window_draw(GtkWidget *widget, cairo_t *cr);

static gpointer ags_animation_window_parent_class = NULL;

GType
ags_animation_window_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_animation_window = 0;

    static const GTypeInfo ags_animation_window_info = {
      sizeof (AgsAnimationWindowClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_animation_window_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAnimationWindow),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_animation_window_init,
    };

    ags_type_animation_window = g_type_register_static(GTK_TYPE_WINDOW,
						       "AgsAnimationWindow", &ags_animation_window_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_animation_window);
  }

  return g_define_type_id__volatile;
}

void
ags_animation_window_class_init(AgsAnimationWindowClass *animation_window)
{
  GtkWidgetClass *widget;
  
  ags_animation_window_parent_class = g_type_class_peek_parent(animation_window);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) animation_window;

  widget->draw = ags_animation_window_draw;
//  widget->show = ags_animation_window_show;
}

void
ags_animation_window_init(AgsAnimationWindow *animation_window)
{
  cairo_surface_t *surface;
  
#if defined(AGS_W32API)
  AgsApplicationContext *application_context;
      
  gchar *app_dir;
#endif
  
  gchar *filename;
  gchar *str;
  
  unsigned char *image_data;

  int width, height;
  int stride;
  
  g_object_set(animation_window,
	       "app-paintable", TRUE,
	       "decorated", FALSE,
	       "window-position", GTK_WIN_POS_CENTER,
	       NULL);

  animation_window->message_count = 0;

  /* create gdk cairo graphics context */
#ifdef AGS_ANIMATION_FILENAME
  filename = g_strdup(AGS_ANIMATION_FILENAME);
#else
  if((filename = getenv("AGS_ANIMATION_FILENAME")) == NULL){
#if defined(AGS_W32API)
    application_context = ags_application_context_get_instance();

    app_dir = NULL;
          
    if(strlen(application_context->argv[0]) > strlen("\\gsequencer.exe")){
      app_dir = g_strndup(application_context->argv[0],
			  strlen(application_context->argv[0]) - strlen("\\gsequencer.exe"));
    }

    filename = g_strdup_printf("%s\\share\\gsequencer\\images\\gsequencer-800x450.png",
			       g_get_current_dir());
    
    if(!g_file_test(filename,
		    G_FILE_TEST_IS_REGULAR)){
      g_free(filename);

      if(g_path_is_absolute(app_dir)){
	filename = g_strdup_printf("%s\\%s",
				   app_dir,
				   "\\share\\gsequencer\\images\\gsequencer-800x450.png");
      }else{
	filename = g_strdup_printf("%s\\%s\\%s",
				   g_get_current_dir(),
				   app_dir,
				   "\\share\\gsequencer\\images\\gsequencer-800x450.png");
      }
    }
#else
    filename = g_strdup_printf("%s%s", AGS_DATA_DIR, "/gsequencer/images/gsequencer-800x450.png");
#endif
  }else{
    filename = g_strdup(filename);
  }
#endif

  animation_window->filename = filename;

  surface = NULL;
  
  image_data = NULL;
  
  width = 800;
  height = 600;

  stride = 2 * width;
  
  animation_window->image_size = stride * height;

  surface = NULL;

  if(filename != NULL){
    cairo_format_t format;
    
    surface = cairo_image_surface_create_from_png(filename);

    image_data = cairo_image_surface_get_data(surface);

    format = cairo_image_surface_get_format(surface);

    width = cairo_image_surface_get_width(surface);
    height = cairo_image_surface_get_height(surface);

    stride = cairo_format_stride_for_width(format,
					   width);
  
    animation_window->image_size = stride * height;
  }
  
  animation_window->bg_data = (unsigned char *) malloc(animation_window->image_size * sizeof(unsigned char));
  animation_window->cache_data = (unsigned char *) malloc(animation_window->image_size * sizeof(unsigned char));

  if(surface != NULL){
    if(image_data != NULL){
      memcpy(animation_window->bg_data, image_data, animation_window->image_size * sizeof(unsigned char));
    }
    
    cairo_surface_destroy(surface);
  }
  
  animation_window->text_box_x0 = 4;
  animation_window->text_box_y0 = 220;

  if((str = getenv("AGS_ANIMATION_TEXT_BOX_X0")) != 0){
    animation_window->text_box_x0 = g_ascii_strtoull(str,
						     NULL,
						     10);
  }

  if((str = getenv("AGS_ANIMATION_TEXT_BOX_Y0")) != 0){
    animation_window->text_box_y0 = g_ascii_strtoull(str,
						     NULL,
						     10);
  }

  animation_window->text_color = g_new0(GdkRGBA,
					1);

  animation_window->text_color->red = 0.680067002;
  animation_window->text_color->green = 1.0;
  animation_window->text_color->blue = 0.998324958;
  animation_window->text_color->alpha = 1.0;
  
  if((str = getenv("AGS_ANIMATION_TEXT_COLOR")) != 0){
    gdk_rgba_parse(animation_window->text_color,
		   str);
  }

  gtk_widget_set_size_request((GtkWidget *) animation_window,
			      800, 450);

  g_timeout_add(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0, (GSourceFunc) ags_animation_window_progress_timeout, (gpointer) animation_window);
}

void
ags_animation_window_show(GtkWidget *widget)
{
  /* call parent */
  GTK_WIDGET_CLASS(ags_animation_window_parent_class)->show(widget);
}

gboolean
ags_animation_window_draw(GtkWidget *widget, cairo_t *cr)
{
  AgsAnimationWindow *animation_window;
  
  AgsLog *log;

  PangoLayout *layout;
  PangoFontDescription *desc;
    
  cairo_surface_t *surface;

  GList *start_list, *list;
  
  unsigned char *image_data;

  gchar *font_name;

  gdouble x0, y0;
  guint i, i_stop;

  GRecMutex *log_mutex;

  animation_window = AGS_ANIMATION_WINDOW(widget);

  log = ags_log_get_instance();

  log_mutex = AGS_LOG_GET_OBJ_MUTEX(log);
  
  list = 
    start_list = ags_log_get_messages(log);
  
  i_stop = g_list_length(start_list);

#if 0
  if(animation_window->message_count >= i_stop){
    return;
  }
#endif
  
  surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24,
				       800, 600);

  if((image_data = cairo_image_surface_get_data(surface)) != NULL){
    memcpy(image_data, animation_window->bg_data, animation_window->image_size * sizeof(unsigned char));
  }

  cairo_set_source_surface(cr, surface, 0, 0);
  cairo_paint(cr);    

  g_object_get(gtk_settings_get_default(),
	       "gtk-font-name", &font_name,
	       NULL);

  /*  */
  x0 = (gdouble) animation_window->text_box_x0;
  y0 = (gdouble) animation_window->text_box_y0 + (i_stop * 12.0);

  cairo_set_source_rgba(cr,
			animation_window->text_color->red,
			animation_window->text_color->green,
			animation_window->text_color->blue,
			animation_window->text_color->alpha);
  
  /* text */
  layout = pango_cairo_create_layout(cr);
  pango_layout_set_text(layout,
			"...",
			-1);
  desc = pango_font_description_from_string(font_name);
  pango_font_description_set_size(desc,
				  9 * PANGO_SCALE);
  pango_layout_set_font_description(layout,
				    desc);
  pango_font_description_free(desc);    

  cairo_move_to(cr,
		x0,
		y0 + 12.0);

  pango_cairo_show_layout(cr,
			  layout);

  g_object_unref(layout);

  for(i = 0; i < i_stop; i++){
    gchar *str;
    
    g_rec_mutex_lock(log_mutex);

    str = g_strdup(list->data);
    
    list = list->next;

    g_rec_mutex_unlock(log_mutex);

    /* text */
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout,
			  str,
			  -1);
    desc = pango_font_description_from_string(font_name);
    pango_font_description_set_size(desc,
				    9 * PANGO_SCALE);
    pango_layout_set_font_description(layout,
				      desc);
    pango_font_description_free(desc);    

    cairo_move_to(cr,
		  x0,
		  y0);

    pango_cairo_show_layout(cr,
			    layout);

    g_object_unref(layout);

    g_free(str);
    
    y0 -= 12.0;
  }

  animation_window->message_count = i_stop;

  g_free(font_name);
  
  cairo_surface_mark_dirty(cairo_get_target(cr));
//  cairo_destroy(cr);

  cairo_surface_destroy(surface);

  return(FALSE);
}

gboolean
ags_animation_window_progress_timeout(AgsAnimationWindow *animation_window)
{
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  if(ags_ui_provider_get_show_animation(AGS_UI_PROVIDER(application_context))){
    AgsLog *log;
    
    GList *start_list;

    guint i_stop;
    
    log = ags_log_get_instance();

    start_list = ags_log_get_messages(log);
  
    i_stop = g_list_length(start_list);
    
    if(animation_window->message_count < i_stop){
      gtk_widget_queue_draw((GtkWidget *) animation_window);
    }

    return(TRUE);
  }else{    
    gtk_widget_hide((GtkWidget *) animation_window);

    ags_ui_provider_setup_completed(AGS_UI_PROVIDER(application_context));
    
    return(FALSE);
  }
}

/**
 * ags_animation_window_new:
 *
 * Creates an #AgsAnimationWindow
 *
 * Returns: a new #AgsAnimationWindow
 *
 * Since: 3.0.0
 */
AgsAnimationWindow*
ags_animation_window_new()
{
  AgsAnimationWindow *animation_window;

  animation_window = (AgsAnimationWindow *) g_object_new(AGS_TYPE_ANIMATION_WINDOW,
							 NULL);

  return(animation_window);
}
