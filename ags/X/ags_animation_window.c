/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/X/ags_animation_window.h>

#include <ags/libags.h>

#include <ags/X/ags_ui_provider.h>

void ags_animation_window_class_init(AgsAnimationWindowClass *animation_window);
void ags_animation_window_init(AgsAnimationWindow *animation_window);
void ags_animation_window_show(GtkWidget *widget);

gboolean ags_animation_window_expose(GtkWidget *widget,
				     GdkEventExpose *event);

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

  widget = (GtkWidgetClass *) animation_window;

  widget->expose_event = ags_animation_window_expose;
//  widget->show = ags_animation_window_show;
}

void
ags_animation_window_init(AgsAnimationWindow *animation_window)
{
  cairo_surface_t *surface;
  
  gchar *filename;
  
  unsigned char *image_data;

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
    filename = g_strdup_printf("%s%s", DESTDIR, "/gsequencer/images/ags_supermoon-800x450.png");
  }else{
    filename = g_strdup(filename);
  }
#endif

  animation_window->filename = filename;

  animation_window->image_size = 3 * 800 * 600;
  
  animation_window->bg_data = (unsigned char *) malloc(animation_window->image_size * sizeof(unsigned char));
  animation_window->cache_data = (unsigned char *) malloc(animation_window->image_size * sizeof(unsigned char));

  if(filename != NULL){
    surface = cairo_image_surface_create_from_png(filename);

    image_data = cairo_image_surface_get_data(surface);

    if(image_data != NULL){
      memcpy(animation_window->bg_data, image_data, animation_window->image_size * sizeof(unsigned char));
    }
    
    cairo_surface_destroy(surface);
  }
  
  gtk_widget_set_size_request((GtkWidget *) animation_window,
			      800, 450);

  g_timeout_add(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0, (GSourceFunc) ags_animation_window_progress_timeout, (gpointer) animation_window);
}

gboolean
ags_animation_window_expose(GtkWidget *widget,
			    GdkEventExpose *event)
{
  ags_animation_window_draw(AGS_ANIMATION_WINDOW(widget));

  return(FALSE);
}

void
ags_animation_window_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_animation_window_parent_class)->show(widget);
}

void
ags_animation_window_draw(AgsAnimationWindow *animation_window)
{
  AgsLog *log;

  PangoLayout *layout;
  PangoFontDescription *desc;
    
  cairo_t *cr;
  cairo_surface_t *surface;

  GList *start_list, *list;
  
  unsigned char *image_data;

  gchar *font_name;

  gdouble x0, y0;
  guint i, i_stop;
  
  if(!AGS_IS_ANIMATION_WINDOW(animation_window)){
    return;
  }

  log = ags_log_get_instance();

  list = 
    start_list = ags_log_get_messages(log);
  
  i_stop = g_list_length(start_list);

#if 0
  if(animation_window->message_count >= i_stop){
    return;
  }
#endif
  
  cr = gdk_cairo_create(GTK_WIDGET(animation_window)->window);

  if(cr == NULL){
    return;
  }

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
  x0 = 4.0;
  y0 = 4.0 + (i_stop * 12.0);

  cairo_set_source_rgb(cr,
		       1.0,
		       0.0,
		       1.0);
  
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
    
    pthread_mutex_lock(log->mutex);

    str = g_strdup(list->data);
    
    list = list->next;

    pthread_mutex_unlock(log->mutex);

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
  cairo_destroy(cr);

  cairo_surface_destroy(surface);
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
      ags_animation_window_draw(animation_window);
    }

    return(TRUE);
  }else{
    gtk_widget_hide(animation_window);
    gtk_widget_show_all(ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context)));    

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
 * Since: 2.2.33
 */
AgsAnimationWindow*
ags_animation_window_new()
{
  AgsAnimationWindow *animation_window;

  animation_window = (AgsAnimationWindow *) g_object_new(AGS_TYPE_ANIMATION_WINDOW,
							 NULL);

  return(animation_window);
}
