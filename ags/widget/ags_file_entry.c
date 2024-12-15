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

#include "ags_file_entry.h"

#include <pango/pango.h>

#ifndef __APPLE__
#include <pango/pangofc-fontmap.h>
#endif

#include <glib/gstdio.h>

#include <gdk/gdkkeysyms.h>

#include <ags/i18n.h>

void ags_file_entry_class_init(AgsFileEntryClass *file_entry);
void ags_file_entry_orientable_interface_init(GtkOrientableIface *orientable);
void ags_file_entry_init(AgsFileEntry *file_entry);
void ags_file_entry_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_file_entry_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_file_entry_dispose(GObject *gobject);
void ags_file_entry_finalize(GObject *gobject);

void ags_file_entry_realize(GtkWidget *widget);
void ags_file_entry_unrealize(GtkWidget *widget);

void ags_file_entry_focus_out(GtkWidget *widget);
void ags_file_entry_focus_in(GtkWidget *widget);

void ags_file_entry_commit_handler(GtkIMContext *context,
				   const char *str,
				   AgsFileEntry *file_entry);
void ags_file_entry_preedit_start_handler(GtkIMContext *context,
					  AgsFileEntry *file_entry);
void ags_file_entry_preedit_changed_handler(GtkIMContext *context,
					    AgsFileEntry *file_entry);
gboolean ags_file_entry_retrieve_surrounding_handler(GtkIMContext *context,
						     AgsFileEntry *file_entry);
gboolean ags_file_entry_delete_surrounding_handler(GtkIMContext *context,
						   int offset,
						   int n_chars,
						   AgsFileEntry *file_entry);

gboolean ags_file_entry_gesture_click_pressed_callback(GtkGestureClick *event_controller,
						       gint n_press,
						       gdouble x,
						       gdouble y,
						       AgsFileEntry *file_entry);
gboolean ags_file_entry_gesture_click_released_callback(GtkGestureClick *event_controller,
							gint n_press,
							gdouble x,
							gdouble y,
							AgsFileEntry *file_entry);

gboolean ags_file_entry_key_pressed_callback(GtkEventControllerKey *event_controller,
					     guint keyval,
					     guint keycode,
					     GdkModifierType state,
					     AgsFileEntry *file_entry);
gboolean ags_file_entry_key_released_callback(GtkEventControllerKey *event_controller,
					      guint keyval,
					      guint keycode,
					      GdkModifierType state,
					      AgsFileEntry *file_entry);
gboolean ags_file_entry_modifiers_callback(GtkEventControllerKey *event_controller,
					   GdkModifierType keyval,
					   AgsFileEntry *file_entry);

void ags_file_entry_key_controller_im_update(GtkEventControllerKey *controller,
					     AgsFileEntry *file_entry);

void ags_file_entry_draw_callback(GtkWidget *drawing_area,
				  cairo_t *cr,
				  int width, int height,
				  AgsFileEntry *file_entry);

void ags_file_entry_edit_drop_down_callback(GObject *gobject,
					    GParamSpec *pspec,
					    AgsFileEntry *file_entry);

void ags_file_entry_real_activate(AgsFileEntry *file_entry);

/**
 * SECTION:ags_file_entry
 * @short_description: a file entry widget
 * @title: AgsFileEntry
 * @section_id:
 * @include: ags/widget/ags_file_entry.h
 *
 * #AgsFileEntry is a entry widget to open or save files.
 */

enum{
  ACTIVATE,
  LAST_SIGNAL,
};

enum{
  PROP_0, 
  PROP_FILENAME,
  PROP_IM_MODULE,
};

static gpointer ags_file_entry_parent_class = NULL;
static guint file_entry_signals[LAST_SIGNAL];

GType
ags_file_entry_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_file_entry = 0;

    static const GTypeInfo ags_file_entry_info = {
      sizeof(AgsFileEntryClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_file_entry_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFileEntry),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_file_entry_init,
    };
    
    ags_type_file_entry = g_type_register_static(GTK_TYPE_BOX,
						 "AgsFileEntry", &ags_file_entry_info,
						 0);    
    
    g_once_init_leave(&g_define_type_id__static, ags_type_file_entry);
  }

  return(g_define_type_id__static);
}

void
ags_file_entry_class_init(AgsFileEntryClass *file_entry)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_file_entry_parent_class = g_type_class_peek_parent(file_entry);

  /* GObjectClass */
  gobject = (GObjectClass *) file_entry;

  gobject->set_property = ags_file_entry_set_property;
  gobject->get_property = ags_file_entry_get_property;

  gobject->dispose = ags_file_entry_dispose;
  gobject->finalize = ags_file_entry_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) file_entry;

  widget->realize = ags_file_entry_realize;
  widget->unrealize = ags_file_entry_unrealize;
  
  /* properties */
  /**
   * AgsFileEntry:filename:
   *
   * The file widget.
   * 
   * Since: 7.5.0
   */
  param_spec = g_param_spec_string("filename",
				   "filename",
				   "The filename",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsFileEntry:im-module:
   *
   * The file widget.
   * 
   * Since: 7.5.0
   */
  param_spec = g_param_spec_string("im-module",
				   "im-module",
				   "The im-module",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_IM_MODULE,
				  param_spec);
  
  /* AgsFileEntry */
  file_entry->activate = ags_file_entry_real_activate;
  
  /* signals */
  /**
   * AgsFileEntry::activate:
   * @file_entry: the #AgsFileEntry
   *
   * The ::activate signal notifies about window interaction.
   *
   * Since: 7.5.0
   */
  file_entry_signals[ACTIVATE] =
    g_signal_new("activate",
		 G_TYPE_FROM_CLASS(file_entry),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileEntryClass, activate),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_file_entry_init(AgsFileEntry *file_entry)
{
  GtkEventController *event_controller;
  
  static const gchar * const edit_strv[] = {
    "insert",
    "select all",
    "copy",
    "cut",
    "paste",
    NULL
  };
  
  file_entry->flags = 0;
  file_entry->key_mask = 0;

  gtk_box_set_spacing((GtkBox *) file_entry,
		      6);

  file_entry->current_keyval = 0;
  file_entry->keyval_timestamp = 0;

  file_entry->im_module = NULL;
  file_entry->im_context = gtk_im_multicontext_new();

  g_signal_connect(file_entry->im_context, "commit",
		   G_CALLBACK(ags_file_entry_commit_handler), file_entry);
  
  g_signal_connect(file_entry->im_context, "preedit-start",
		   G_CALLBACK(ags_file_entry_preedit_start_handler), file_entry);

  g_signal_connect(file_entry->im_context, "preedit-changed",
		   G_CALLBACK(ags_file_entry_preedit_changed_handler), file_entry);

  g_signal_connect(file_entry->im_context, "retrieve-surrounding",
		   G_CALLBACK(ags_file_entry_retrieve_surrounding_handler), file_entry);

  g_signal_connect(file_entry->im_context, "delete-surrounding",
		   G_CALLBACK(ags_file_entry_delete_surrounding_handler), file_entry);
  
  file_entry->cursor_position = -1;
  
  file_entry->max_filename_length = AGS_FILE_ENTRY_DEFAULT_MAX_FILENAME_LENGTH;
  
  file_entry->filename = NULL;
  
  file_entry->auto_completion_filename = NULL;

  file_entry->font_size = 11;
  file_entry->xalign = 0.0;

  file_entry->selection_offset_x0 = NULL;
  file_entry->selection_offset_x1 = NULL;

  file_entry->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();  

  gtk_widget_set_can_focus((GtkWidget *) file_entry->drawing_area,
			   TRUE);
  gtk_widget_set_focusable((GtkWidget *) file_entry->drawing_area,
			   TRUE);

  gtk_widget_set_halign((GtkWidget *) file_entry->drawing_area,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand((GtkWidget *) file_entry->drawing_area,
			 TRUE);  

  gtk_box_append((GtkBox *) file_entry,
		 (GtkWidget *) file_entry->drawing_area);

  /* focus */
  event_controller = gtk_event_controller_focus_new();
  gtk_widget_add_controller((GtkWidget *) file_entry,
			    event_controller);

  g_signal_connect_swapped(event_controller, "enter",
			   G_CALLBACK(ags_file_entry_focus_in), file_entry);

  g_signal_connect_swapped(event_controller, "leave",
			   G_CALLBACK(ags_file_entry_focus_out), file_entry);
  
  /* click */
  event_controller =  (GtkEventController *) gtk_gesture_click_new();
  gtk_widget_add_controller((GtkWidget *) file_entry->drawing_area,
			    event_controller);

  g_signal_connect(event_controller, "pressed",
		   G_CALLBACK(ags_file_entry_gesture_click_pressed_callback), file_entry);

  g_signal_connect(event_controller, "released",
		   G_CALLBACK(ags_file_entry_gesture_click_released_callback), file_entry);

  /* key */
  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) file_entry->drawing_area,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_file_entry_key_pressed_callback), file_entry);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_file_entry_key_released_callback), file_entry);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_file_entry_modifiers_callback), file_entry);

  g_signal_connect(event_controller, "im-update",
		   G_CALLBACK(ags_file_entry_key_controller_im_update), file_entry);
  
  gtk_event_controller_key_set_im_context(GTK_EVENT_CONTROLLER_KEY(event_controller),
					  file_entry->im_context);
  
  /* draw */
  gtk_drawing_area_set_draw_func(file_entry->drawing_area,
				 (GtkDrawingAreaDrawFunc) ags_file_entry_draw_callback,
				 file_entry,
				 NULL);

  /* edit drop down */
  file_entry->edit_drop_down = (GtkDropDown *) gtk_drop_down_new_from_strings(edit_strv);
  gtk_box_append((GtkBox *) file_entry,
		 (GtkWidget *) file_entry->edit_drop_down);

  g_signal_connect(file_entry->edit_drop_down, "notify::selected",
		   G_CALLBACK(ags_file_entry_edit_drop_down_callback), file_entry);
}

void
ags_file_entry_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsFileEntry *file_entry;

  file_entry = AGS_FILE_ENTRY(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      if(file_entry->filename == filename){
	return;
      }

      file_entry->filename = g_strdup_printf("%s",
					     filename);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_entry_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsFileEntry *file_entry;

  file_entry = AGS_FILE_ENTRY(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, file_entry->filename);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_entry_dispose(GObject *gobject)
{  
  /* call parent */
  G_OBJECT_CLASS(ags_file_entry_parent_class)->dispose(gobject);
}

void
ags_file_entry_finalize(GObject *gobject)
{
  AgsFileEntry *file_entry;
  
  file_entry = AGS_FILE_ENTRY(gobject);

  g_free(file_entry->filename);
  g_free(file_entry->auto_completion_filename);
  
  /* call parent */
  G_OBJECT_CLASS(ags_file_entry_parent_class)->finalize(gobject);
}

void
ags_file_entry_realize(GtkWidget *widget)
{
  /* call parent */
  GTK_WIDGET_CLASS(ags_file_entry_parent_class)->realize(widget);

  if(gtk_widget_is_sensitive(widget)){
    gtk_im_context_set_client_widget(AGS_FILE_ENTRY(widget)->im_context,
				     widget);
  }
}

void
ags_file_entry_unrealize(GtkWidget *widget)
{
  gtk_im_context_set_client_widget(AGS_FILE_ENTRY(widget)->im_context,
				   NULL);
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_file_entry_parent_class)->unrealize(widget);
}

void
ags_file_entry_focus_in(GtkWidget *widget)
{
  AGS_FILE_ENTRY(widget)->cursor_position = strlen(AGS_FILE_ENTRY(widget)->filename);
  
  gtk_widget_queue_draw(widget);
  
  gtk_im_context_focus_in(AGS_FILE_ENTRY(widget)->im_context);
}

void
ags_file_entry_focus_out(GtkWidget *widget)
{
  AGS_FILE_ENTRY(widget)->cursor_position = -1;
  
  gtk_widget_queue_draw(widget);
  
  gtk_im_context_focus_out(AGS_FILE_ENTRY(widget)->im_context);
}

void
ags_file_entry_commit_handler(GtkIMContext *context,
			      const char *str,
			      AgsFileEntry *file_entry)
{
  gchar *current_filename;
  
  gint filename_len;
  gint len;
  
  g_message("commit %s", str);

  filename_len = strlen(file_entry->filename);
  
  len = strlen(str);
  
  if(file_entry->cursor_position >= 0){
    if(file_entry->cursor_position > filename_len){
      file_entry->cursor_position = filename_len;
    }
    
    if(file_entry->cursor_position < filename_len){
      gchar *pre_str;
      gchar *post_str;

      pre_str = NULL;
      post_str = NULL;

      if(file_entry->cursor_position > 0){
	pre_str = g_strndup(file_entry->filename,
			    file_entry->cursor_position);
      }

      if(file_entry->cursor_position < filename_len){
	post_str = g_strdup(file_entry->filename + file_entry->cursor_position);
      }

      if(pre_str != NULL && post_str != NULL){
	current_filename = g_strdup_printf("%s%s%s",
					   pre_str,
					   str,
					   post_str);
      }else if(pre_str != NULL){
	current_filename = g_strdup_printf("%s%s",
					   pre_str,
					   str);
      }else if(post_str != NULL){
	current_filename = g_strdup_printf("%s%s",
					   str,
					   post_str);
      }else{
	current_filename = g_strdup_printf("%s",
					   str);
      }
      
      ags_file_entry_set_filename(file_entry,
				  current_filename);

      file_entry->cursor_position += len;
      
      g_free(current_filename);
      g_free(pre_str);      
      g_free(post_str);      
    }else{
      current_filename = g_strdup_printf("%s%s",
					 file_entry->filename,
					 str);

      ags_file_entry_set_filename(file_entry,
				  current_filename);

      file_entry->cursor_position += len;

      g_free(current_filename);
    }
  }else{
    current_filename = g_strdup_printf("%s%s",
				       file_entry->filename,
				       str);

    ags_file_entry_set_filename(file_entry,
				current_filename);

    g_free(current_filename);
  }
}

void
ags_file_entry_preedit_start_handler(GtkIMContext *context,
				     AgsFileEntry *file_entry)
{
}

void
ags_file_entry_preedit_changed_handler(GtkIMContext *context,
				       AgsFileEntry *file_entry)
{
  PangoAttrList *attrs;

  char *str;

  int cursor_pos;

  attrs = NULL;

  str = NULL;
  
  gtk_im_context_get_preedit_string(context,
				    &str,
				    &attrs,
				    &cursor_pos);
  
  pango_attr_list_unref(attrs);

  g_free(str);
}

gboolean
ags_file_entry_retrieve_surrounding_handler(GtkIMContext *context,
					    AgsFileEntry *file_entry)
{
  char *text;

  int cursor_pos;
  int anchor_pos;

  text = NULL;
  
  cursor_pos = -1;
  anchor_pos = -1;
  
  gtk_im_context_set_surrounding_with_selection(context,
						text,
						-1,
						cursor_pos,
						anchor_pos);

  g_free(text);

  return(TRUE);
}

gboolean
ags_file_entry_delete_surrounding_handler(GtkIMContext *context,
					  int offset,
					  int n_chars,
					  AgsFileEntry *file_entry)
{
  return(TRUE);
}

gboolean
ags_file_entry_im_context_filter_keypress(AgsFileEntry *file_entry,
                                          GdkEvent *event)
{
  g_return_val_if_fail(AGS_IS_FILE_ENTRY(file_entry), FALSE);
  
  return(gtk_im_context_filter_keypress(file_entry->im_context, event));
}

gboolean
ags_file_entry_gesture_click_pressed_callback(GtkGestureClick *event_controller,
					      gint n_press,
					      gdouble x,
					      gdouble y,
					      AgsFileEntry *file_entry)
{
  g_message("focus");

  gtk_widget_grab_focus((GtkWidget *) file_entry->drawing_area);

  return(TRUE);
}

gboolean
ags_file_entry_gesture_click_released_callback(GtkGestureClick *event_controller,
					       gint n_press,
					       gdouble x,
					       gdouble y,
					       AgsFileEntry *file_entry)
{
  g_message("focus");

  gtk_widget_grab_focus((GtkWidget *) file_entry->drawing_area);

  return(TRUE);
}

gboolean
ags_file_entry_key_pressed_callback(GtkEventControllerKey *event_controller,
				    guint keyval,
				    guint keycode,
				    GdkModifierType state,
				    AgsFileEntry *file_entry)
{
  gchar outbuf[8];

  guint32 unicode_val;
  gint n;
  gint64 time_now;

  gboolean key_handled;

  GError *error;

  unicode_val = gdk_keyval_to_unicode(keyval);
    
  memset(outbuf, 0, 8 * sizeof(gchar));

  time_now = g_get_monotonic_time();

  key_handled = FALSE;

  /* control */
  if(keyval == GDK_KEY_ISO_Enter){
    g_message("control: ISO Enter");

    key_handled = TRUE;
  }else if(keyval == GDK_KEY_Delete){
    g_message("control: delete");

    key_handled = TRUE;
  }else if(keyval == GDK_KEY_Clear){
    g_message("control: clear");

    //TODO:JK: implement me
    
    key_handled = TRUE;
  }else if(keyval == GDK_KEY_BackSpace){
    gchar *filename;

    gint cursor_position;
    gint filename_len;

    gint i;
    gint j;
      
    g_message("control: BackSpace");
    
    filename_len = strlen(file_entry->filename);

    cursor_position = -1;

    if(file_entry->cursor_position > 0){
      if(file_entry->cursor_position > filename_len){
	file_entry->cursor_position = filename_len;
      }
    }

    if(file_entry->cursor_position > 0){
      cursor_position = file_entry->cursor_position;
    }else{
      cursor_position = filename_len;
    }
    
    if(cursor_position > 0){
      filename = file_entry->filename;

      i = 0;
      j = 0;
      
      do{
	gunichar ustr[2];

	gchar *tmp_str;

	glong items_read;
	glong items_written;
	
	ustr[0] = g_utf8_get_char(filename);
	ustr[1] = 0x0;

	items_read = 0;
	items_written = 0;
	
	error = NULL;
	tmp_str = g_ucs4_to_utf8(ustr,
				 -1,
				 &items_read,
				 &items_written,
				 &error);

	if(error != NULL){
	  g_error_free(error);
	}
	
	if(items_written > 0){
	  filename += items_written;

	  j += items_written;
	}else{
	  filename++;
	  j++;
	}
	
	i++;
	
	g_free(tmp_str);

	if(j == cursor_position){
	  gchar *current_filename;
	  gchar *pre_str;
	  gchar *post_str;

	  current_filename = NULL;
	  
	  pre_str = NULL;
	  post_str = NULL;
	  
	  if(items_written > 0){
	    if(items_written < j){
	      pre_str = g_strndup(file_entry->filename,
				  j - items_written);
	    }

	    if(j < filename_len){
	      post_str = g_strdup(file_entry->filename + j);
	    }
	  }else{
	    if(items_written < j){
	      pre_str = g_strndup(file_entry->filename,
				  j - 1);
	    }

	    if(j < filename_len){
	      post_str = g_strdup(file_entry->filename + j);
	    }
	  }

	  if(pre_str != NULL && post_str != NULL){
	    current_filename = g_strdup_printf("%s%s",
					       pre_str,
					       post_str);
	  }else if(pre_str != NULL){
	    current_filename = g_strdup(pre_str);
	  }else if(post_str != NULL){
	    current_filename = g_strdup(post_str);
	  }else{
	    current_filename = NULL;
	  }

	  ags_file_entry_set_filename(file_entry,
				      current_filename);

	  if(items_written > 0){	    
	    cursor_position -= items_written;
	  }else{
	    cursor_position -= 1;
	  }

	  g_free(current_filename);
	  
	  g_free(post_str);
	  g_free(pre_str);
	  
	  break;
	}
      }while(filename[0] != '\0' && i < AGS_FILE_ENTRY_DEFAULT_MAX_FILENAME_LENGTH);
    }

    if(file_entry->cursor_position > 0){
      file_entry->cursor_position = cursor_position;
    }
    
    key_handled = TRUE;
  }else if(keyval == GDK_KEY_Left){
    gchar *filename;

    gint filename_len;

    gint i;
    gint j;
    
    filename_len = strlen(file_entry->filename);

    if(file_entry->cursor_position > 0){
      if(file_entry->cursor_position > filename_len){
	file_entry->cursor_position = filename_len;
      }

      filename = file_entry->filename;

      i = 0;
      j = 0;
      
      do{
	gunichar ustr[2];

	gchar *tmp_str;

	glong items_read;
	glong items_written;
	
	ustr[0] = g_utf8_get_char(filename);
	ustr[1] = 0x0;

	items_read = 0;
	items_written = 0;
	
	error = NULL;
	tmp_str = g_ucs4_to_utf8(ustr,
				 -1,
				 &items_read,
				 &items_written,
				 &error);

	if(error != NULL){
	  g_error_free(error);
	}
	
	if(items_written > 0){
	  filename += items_written;

	  j += items_written;
	}else{
	  filename++;
	  j++;
	}
	
	i++;
	
	g_free(tmp_str);

	if(j == file_entry->cursor_position){
	  if(items_written > 0){
	    file_entry->cursor_position -= items_written;
	  }else{
	    file_entry->cursor_position -= 1;
	  }
	  
	  break;
	}
      }while(filename[0] != '\0' && i < AGS_FILE_ENTRY_DEFAULT_MAX_FILENAME_LENGTH);
    }
    
    key_handled = TRUE;
  }else if(keyval == GDK_KEY_Right){
    gchar *filename;

    gint filename_len;

    gint i;
    gint j;

    filename_len = strlen(file_entry->filename);

    if(file_entry->cursor_position > 0){
      if(file_entry->cursor_position > filename_len){
	file_entry->cursor_position = filename_len;
      }

      filename = file_entry->filename;

      i = 0;
      j = 0;
      
      do{
	gunichar ustr[2];

	gchar *tmp_str;

	glong items_read;
	glong items_written;
	
	ustr[0] = g_utf8_get_char(filename);
	ustr[1] = 0x0;

	items_read = 0;
	items_written = 0;
	
	error = NULL;
	tmp_str = g_ucs4_to_utf8(ustr,
				 -1,
				 &items_read,
				 &items_written,
				 &error);

	if(error != NULL){
	  g_error_free(error);
	}
	
	if(items_written > 0){
	  filename += items_written;

	  j += items_written;
	}else{
	  filename++;
	  j++;
	}
	
	i++;
	
	g_free(tmp_str);

	if(j == file_entry->cursor_position){
	  if(filename[0] != '\0'){
	    ustr[0] = g_utf8_get_char(filename);
	    ustr[1] = 0x0;

	    items_read = 0;
	    items_written = 0;
	
	    error = NULL;
	    tmp_str = g_ucs4_to_utf8(ustr,
				     -1,
				     &items_read,
				     &items_written,
				     &error);

	    if(error != NULL){
	      g_error_free(error);
	    }
	  
	    if(items_written > 0){
	      file_entry->cursor_position += items_written;
	    }else{
	      file_entry->cursor_position += 1;
	    }
	  }
	  
	  break;
	}
      }while(filename[0] != '\0' && i < AGS_FILE_ENTRY_DEFAULT_MAX_FILENAME_LENGTH);
    }
        
    key_handled = TRUE;
  }else if(keyval == GDK_KEY_Shift_Lock){
    //TODO:JK: implement me
    
    key_handled = TRUE;
  }else if(keyval == GDK_KEY_Shift_L ||
	   keyval == GDK_KEY_Shift_R){    
    //TODO:JK: implement me
    
    key_handled = TRUE;
  }else if(keyval == GDK_KEY_Control_L ||
	   keyval == GDK_KEY_Control_R){    
    //TODO:JK: implement me
    
    key_handled = TRUE;
  }else if(keyval == GDK_KEY_Tab ||
	   keyval == GDK_KEY_Escape){
    return(FALSE);
  }
  
  file_entry->current_keyval = keyval;
  file_entry->keyval_timestamp = time_now;

  if(key_handled){
    gtk_widget_queue_draw((GtkWidget *) file_entry->drawing_area);
  }
  
  return(key_handled);
}

gboolean
ags_file_entry_key_released_callback(GtkEventControllerKey *event_controller,
				     guint keyval,
				     guint keycode,
				     GdkModifierType state,
				     AgsFileEntry *file_entry)
{
  return(FALSE);
}

gboolean
ags_file_entry_modifiers_callback(GtkEventControllerKey *event_controller,
				  GdkModifierType keyval,
				  AgsFileEntry *file_entry)
{
  return(FALSE);
}

void
ags_file_entry_key_controller_im_update(GtkEventControllerKey *controller,
					AgsFileEntry *file_entry)
{
  
}

void
ags_file_entry_draw_callback(GtkWidget *drawing_area,
			     cairo_t *cr,
			     int width, int height,
			     AgsFileEntry *file_entry)
{
  GtkStyleContext *style_context;
  GtkSettings *settings;

  PangoLayout *layout;
  PangoFontDescription *desc;

  PangoRectangle ink_rect, logical_rect;

  GdkRGBA fg_color;
  GdkRGBA bg_color;
  GdkRGBA shadow_color;
  GdkRGBA text_color;
    
  gchar *font_name;
  gchar *filename;

  gint cursor_position;
  gint filename_len;

  gint i;
  gint j;

  gdouble x_start, y_start;
  gboolean dark_theme;
  gboolean fg_success;
  gboolean bg_success;
  gboolean text_success;
  gboolean shadow_success;

  GError *error;
  
  if(file_entry->filename == NULL){
    return;
  }
  
  style_context = gtk_widget_get_style_context((GtkWidget *) drawing_area);

  settings = gtk_settings_get_default();

  font_name = NULL;
  
  dark_theme = TRUE;
  
  g_object_get(settings,
	       "gtk-font-name", &font_name,
	       "gtk-application-prefer-dark-theme", &dark_theme,
	       NULL);

  /* colors */
  fg_success = gtk_style_context_lookup_color(style_context,
					      "theme_fg_color",
					      &fg_color);
    
  bg_success = gtk_style_context_lookup_color(style_context,
					      "theme_bg_color",
					      &bg_color);
    
  shadow_success = gtk_style_context_lookup_color(style_context,
						  "theme_shadow_color",
						  &shadow_color);
    
  text_success = gtk_style_context_lookup_color(style_context,
						"theme_text_color",
						&text_color);

  if(!fg_success ||
     !bg_success ||
     !shadow_success ||
     !text_success){
    gdk_rgba_parse(&fg_color,
		   "#101010");

    gdk_rgba_parse(&bg_color,
		   "#cbd5d9");

    gdk_rgba_parse(&shadow_color,
		   "#ffffff40");

    gdk_rgba_parse(&text_color,
		   "#1a1a1a");
  }

  cairo_push_group(cr);

  /* background */
  cairo_set_source_rgb(cr,
		       1.0,
		       1.0,
		       1.0);
  cairo_rectangle(cr,
		  (gdouble) 0.0, (gdouble) 0.0,
		  (gdouble) width, (gdouble) height);
  cairo_fill(cr);

  /* cursor */  
  filename_len = strlen(file_entry->filename);
  
  cursor_position = file_entry->cursor_position;

  if(cursor_position > filename_len){
    cursor_position = filename_len;
  }

  if(cursor_position >= 0){
    gdouble x_cursor;
    
    filename = file_entry->filename;

    i = 0;
    j = 0;

    x_cursor = 0.0;

    layout = pango_cairo_create_layout(cr);
      
    do{
      gunichar ustr[2];

      gchar *tmp_str;

      glong items_read;
      glong items_written;
	
      ustr[0] = g_utf8_get_char(filename);
      ustr[1] = 0x0;

      items_read = 0;
      items_written = 0;
	
      error = NULL;
      tmp_str = g_ucs4_to_utf8(ustr,
			       -1,
			       &items_read,
			       &items_written,
			       &error);

      if(error != NULL){
	g_error_free(error);
      }
	
      if(items_written > 0){
	filename += items_written;

	j += items_written;
      }else{
	filename++;
	j++;
      }
	
      i++;

      pango_layout_set_text(layout,
			    tmp_str,
			    -1);

      desc = pango_font_description_from_string(font_name);
      pango_font_description_set_size(desc,
				      file_entry->font_size * PANGO_SCALE);
      pango_layout_set_font_description(layout,
					desc);
      pango_font_description_free(desc);    

      pango_layout_get_extents(layout,
			       &ink_rect,
			       &logical_rect);
      
      g_free(tmp_str);

      if(j == cursor_position){
	x_cursor += ((gdouble) logical_rect.width / PANGO_SCALE);

	cairo_set_source_rgb(cr,
			     0.0,
			     0.0,
			     0.0);
    
	cairo_move_to(cr,
		      x_cursor, height / 4.0 + 19.0);
	cairo_line_to(cr,
		      x_cursor, height / 4.0 - 2.0);
    
	cairo_stroke(cr);
	  
	break;
      }else{
	x_cursor += ((gdouble) logical_rect.width / PANGO_SCALE);
      }
    }while(filename[0] != '\0' && i < AGS_FILE_ENTRY_DEFAULT_MAX_FILENAME_LENGTH);

    if(j != cursor_position){
      cairo_set_source_rgb(cr,
			   0.0,
			   0.0,
			   0.0);
    
      cairo_move_to(cr,
		    x_cursor, 0.0);
      cairo_line_to(cr,
		    x_cursor, height);
    
      cairo_stroke(cr);
    }
      
    g_object_unref(layout);
  }
  
  /* pango */
  layout = pango_cairo_create_layout(cr);
  pango_layout_set_text(layout,
			file_entry->filename,
			-1);
  desc = pango_font_description_from_string(font_name);
  pango_font_description_set_size(desc,
				  file_entry->font_size * PANGO_SCALE);
  pango_layout_set_font_description(layout,
				    desc);
  pango_font_description_free(desc);    

  pango_layout_get_extents(layout,
			   &ink_rect,
			   &logical_rect);

  cairo_set_source_rgb(cr,
		       0.0,
		       0.0,
		       0.0);
  
  x_start = 0.0;
  y_start = 0.0;
  
  cairo_move_to(cr,
		x_start,
		y_start + (logical_rect.height / PANGO_SCALE) / 4.0 + 1.0);
  
  pango_cairo_show_layout(cr,
			  layout);

  g_object_unref(layout);

  /* paint */  
  cairo_pop_group_to_source(cr);
  cairo_paint(cr);

  g_free(font_name);
}

void
ags_file_entry_edit_drop_down_callback(GObject *gobject,
				       GParamSpec *pspec,
				       AgsFileEntry *file_entry)
{
  guint selected;
  
  selected = gtk_drop_down_get_selected(file_entry->edit_drop_down);

  if(selected == 0){
    return;
  }
  
  //TODO:JK: implement me
  
  gtk_drop_down_set_selected(file_entry->edit_drop_down,
			     0);
}

void
ags_file_entry_real_activate(AgsFileEntry *file_entry)
{
  //TODO:JK: implement me
}

/**
 * ags_file_entry_activate:
 * @file_entry: the #AgsFileEntry
 *
 * Activate @file_entry due to user action.
 * 
 * Since: 7.5.0
 */
void
ags_file_entry_activate(AgsFileEntry *file_entry)
{
  g_return_if_fail(AGS_IS_FILE_ENTRY(file_entry));
  
  g_object_ref((GObject *) file_entry);
  g_signal_emit(G_OBJECT(file_entry),
		file_entry_signals[ACTIVATE], 0);
  g_object_unref((GObject *) file_entry);
}

/**
 * ags_file_entry_get_filename:
 * @file_entry: the #AgsFileEntry
 *
 * Get filename of @file_entry.
 *
 * Returns: (transfer full): the filename
 * 
 * Since: 7.5.0
 */
gchar*
ags_file_entry_get_filename(AgsFileEntry *file_entry)
{
  gchar *filename;

  g_return_val_if_fail(AGS_IS_FILE_ENTRY(file_entry), NULL);
  
  filename = NULL;
  
  g_object_get(file_entry,
	       "filename", &filename,
	       NULL);
  
  return(filename);
}

/**
 * ags_file_entry_set_filename:
 * @file_entry: the #AgsFileEntry
 * @filename: the filename
 *
 * Set filename of @file_entry.
 * 
 * Since: 7.5.0
 */
void
ags_file_entry_set_filename(AgsFileEntry *file_entry,
			    gchar *filename)
{
  g_return_if_fail(AGS_IS_FILE_ENTRY(file_entry));

  g_object_set(file_entry,
	       "filename", filename,
	       NULL);

  gtk_widget_queue_draw((GtkWidget *) file_entry->drawing_area);
}

/**
 * ags_file_entry_new:
 *
 * Creates a new instance of #AgsFileEntry.
 *
 * Returns: the new #AgsFileEntry
 *
 * Since: 7.5.0
 */
AgsFileEntry*
ags_file_entry_new()
{
  AgsFileEntry *file_entry;

  file_entry = (AgsFileEntry *) g_object_new(AGS_TYPE_FILE_ENTRY,
					     NULL);
  
  return(file_entry);
}

